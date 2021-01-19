#include <QDebug>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

#include "vcconfig.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static VCConfig * m_pInstance = nullptr;
/*--------------------------------------------------------------------------------------------------------------------*/

VCConfig::VCConfig( QObject * pParent ) : QObject( pParent )
{
    // Find our save slot in the Meta-Object system.
    const QMetaObject * pMetaObject = metaObject();
    for ( int i = 0; i < pMetaObject->methodCount(); i++ )
    {
        QMetaMethod Method = pMetaObject->method( i );
        if ( "save" == Method.name() )
        {
            m_SaveMethod = Method;
            break;
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

VCConfig * VCConfig::instance()
{
    if ( nullptr == m_pInstance )
    {
        m_pInstance = new VCConfig( VCHub::instance() );  // BDP: The parent allows key resolution.
    }

    return m_pInstance;
}
/*--------------------------------------------------------------------------------------------------------------------*/

bool VCConfig::load( const QString & Path )
{
    bool bReturn = false;

    // Ensure the path refers to a writable file.
    if ( QFile::exists( Path ) && QFileInfo( Path ).isWritable() )
    {
        QFile ConfigFile( Path );
        if ( ConfigFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            qDebug() << "Loading config file: " << Path;
            QJsonDocument ConfigDocument = QJsonDocument::fromJson( ConfigFile.readAll() );
            if ( ConfigDocument.isObject() )
            {
                QJsonObject Config = ConfigDocument.object();
                m_Keys.clear();
                const QStringList Keys = Config.keys();
                for ( const auto & Key : Keys )
                {
                    KeyContext Context = keyToContext( Key );

                    // Did the key resolve to a valid object and property name?
                    if ( ( nullptr != Context.first ) && ( !Context.second.isEmpty() ) )
                    {
                        QObject * pObject = Context.first;
                        QString Property = Context.second;

                        // Ensure the property exists.
                        if ( pObject->property( Property.toStdString().c_str() ).isValid() )
                        {
                            // Set the property to the value specified in the file.
                            pObject->setProperty( Property.toStdString().c_str(), Config.value( Key ).toVariant() );

                            // Save the key name to track it now that is has been validated.
                            m_Keys.append( Key );

                            // Use the property's NOTIFY signal to drive saving back the file.
                            const QMetaObject * pMetaObject = pObject->metaObject();
                            for ( int i = 0; i < pMetaObject->propertyCount(); i++ )
                            {
                                QMetaProperty MetaProperty = pMetaObject->property( i );
                                if ( Property == MetaProperty.name() )
                                {
                                    if ( MetaProperty.hasNotifySignal() )
                                    {
                                        // Ensure connections are not duplicated.
                                        connect( pObject, MetaProperty.notifySignal(),
                                                 this, m_SaveMethod, Qt::UniqueConnection );
                                        break;
                                    }
                                    else
                                    {
                                        qWarning() << "No NOTIFY signal for property in config key: " << Key;
                                    }
                                }
                            }
                        }
                        else
                        {
                            qWarning() << "Ignoring config key because the property does not exist: " << Key;
                        }
                    }
                    else
                    {
                        qWarning() << "Ignoring config key because it does not resolve: " << Key;
                    }
                }

                m_Path = Path;
                bReturn = true;
            }
            else
            {
                qWarning() << "Failed to parse config file structure: " << Path;
                bReturn = false;
            }
            ConfigFile.close();
        }
        else
        {
            qWarning() << "Failed to open config file: " << Path;
            bReturn = false;
        }
    }
    else
    {
        qWarning() << "Ignoring config file path because it does not refer to an existing, writable file: " << Path;
        bReturn = false;
    }

    return bReturn;
}
/*--------------------------------------------------------------------------------------------------------------------*/

bool VCConfig::save()
{
    bool bReturn = false;

    // Ensure that a config has been previously loaded.
    if ( ( !m_Path.isEmpty() ) && ( !m_Keys.isEmpty() ) )
    {
        QJsonObject Config;
        for ( const auto & Key : qAsConst( m_Keys ) )
        {
            KeyContext Context = keyToContext( Key );

            // This should always resolve since this is a validated key, but do a sanity check anyway.
            if ( ( nullptr != Context.first ) && ( !Context.second.isEmpty() ) )
            {
                QObject * pObject = Context.first;
                QString Property = Context.second;

                // Record the latest property value.
                Config[ Key ] = QJsonValue::fromVariant( pObject->property( Property.toStdString().c_str() ) );
            }
            else
            {
                qWarning() << "Not saving config key because it does not resolve: " << Key;
            }
        }

        if ( !Config.isEmpty() )
        {
            QFile ConfigFile( m_Path );
            if ( ConfigFile.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text ) )
            {
                ConfigFile.write( QJsonDocument( Config ).toJson() );
                ConfigFile.close();
                qDebug() << "Saved config file: " << m_Path;
                bReturn = true;
            }
            else
            {
                qWarning() << "Failed to open config file when trying to save: " << m_Path;
            }
        }
        else
        {
            qWarning() << "Failed to build config file structure when trying to save";
        }
    }
    else
    {
        qDebug() << "Not saving config because one was not previously loaded";
    }

    return bReturn;
}
/*--------------------------------------------------------------------------------------------------------------------*/

KeyContext VCConfig::keyToContext( const QString & Key )
{
    QObject * pObject = nullptr;
    QString PropertyName;

    // Keys are references into the Meta-Object system using object names, with the property to set at the end.
    QStringList Parts = Key.split( QChar( '.' ) );
    if ( 2 == Parts.size() )
    {
        // Drop the property name.
        PropertyName = Parts.takeLast();

        // Treat the parent as the top-most item in the heirarchy.
        pObject = parent();

        // If the object name is not the parent's, recurse down into the children to find it.
        if ( pObject->objectName() != Parts.first() )
        {
            pObject = pObject->findChild<QObject *>( Parts.first() );
        }
    }

    return KeyContext { pObject, PropertyName };
}
/*--------------------------------------------------------------------------------------------------------------------*/
