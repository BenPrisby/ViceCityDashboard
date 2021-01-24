#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>

#include "vcspotify.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static const QString PLAYER_BASE_URL = "https://api.spotify.com/v1/me/player";
/*--------------------------------------------------------------------------------------------------------------------*/

VCSpotify::VCSpotify( const QString & Name, QObject * pParent ) :
    VCPlugin( Name, pParent ),
    m_bIsActive( false ),
    m_bIsPlaying( false ),
    m_bShuffleEnabled( false ),
    m_bRepeatOneEnabled( false ),
    m_bRepeatAllEnabled( false ),
    m_iTrackPosition( 0 ),
    m_iTrackDuration( 0 ),
    m_iDeviceVolume( 0 ),
    m_Market( QLocale::system().name().split( QChar( '_' ) ).last() )
{
    m_UpdateTimer.setInterval( 1000 );
    m_UpdateTimer.stop();

    // Handle network responses.
    connect( NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCSpotify::handleNetworkReply );

    // Request the initial access token when we are told what the refresh token is and have the client information.
    connect( this, &VCSpotify::clientIDChanged, this, &VCSpotify::refreshAccessToken );
    connect( this, &VCSpotify::clientSecretChanged, this, &VCSpotify::refreshAccessToken );
    connect( this, &VCSpotify::refreshTokenChanged, this, &VCSpotify::refreshAccessToken );

    // Configure a timer to periodically refresh the access token.
    m_AccessTokenRefreshTimer.setInterval( 59 * 60 * 1000 );  // 1 minute less than the standard expiry time
    m_AccessTokenRefreshTimer.setSingleShot( false );
    connect( &m_AccessTokenRefreshTimer, &QTimer::timeout, this, &VCSpotify::refreshAccessToken );

    // Configure a timer to set the state as idle if a response is not received.
    m_InactivityTimer.setInterval( 5 * 1000 );
    m_InactivityTimer.setSingleShot( true );
    connect( &m_InactivityTimer, &QTimer::timeout, this, [=]{
        if ( m_bIsActive )
        {
            m_bIsActive = false;
            emit isActiveChanged();

            // Clear some of the playback state.
            m_iTrackPosition = 0;
            emit trackPositionChanged();
            m_iTrackDuration = 0;
            emit trackDurationChanged();
            m_PlaylistName.clear();
            emit playlistNameChanged();
        }
    });

    // Configure a timer to use as a reference to hold off processing after submitting an action.
    // BDP: This helps with keeping things responsive until the API reports the updated state, after which if there is
    //      still a disagreement, the properties will update as normal.
    m_ActionSubmissionTimer.setInterval( m_UpdateTimer.interval() / 2 );
    m_ActionSubmissionTimer.setSingleShot( true );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refresh()
{
    // Refresh current playback information.
    static QUrl Destination( QString( "%1?market=%2" ).arg( PLAYER_BASE_URL, m_Market ) );
    sendRequest( Destination );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refreshDevices()
{
    if ( !m_AccessTokenAuthorization.isEmpty() )
    {
        static QUrl Destination( QString( "%1/devices" ).arg( PLAYER_BASE_URL ) );
        sendRequest( Destination );
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refreshUserProfile()
{
    if ( !m_AccessTokenAuthorization.isEmpty() )
    {
        static QUrl Destination( "https://api.spotify.com/v1/me" );
        sendRequest( Destination );
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refreshPlaylists()
{
    if ( !m_AccessTokenAuthorization.isEmpty() )
    {
        static QUrl Destination( "https://api.spotify.com/v1/me/playlists" );
        sendRequest( Destination );
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::play( const QString & URI )
{
    QString Destination = QString( "%1/play" ).arg( PLAYER_BASE_URL );
    if ( !m_bIsActive )
    {
        // Idle, so a device must be specified for playing to start.
        if ( !m_PreferredDeviceID.isEmpty() )
        {
            qDebug() << "Spotify playback is not active, so defaulting to playing on device: " << m_PreferredDevice;
            Destination.append( QString( "?device_id=%1" ).arg( m_PreferredDeviceID ) );
        }
        else
        {
            qDebug() << "Spotify playback is not active, but there is no preferred device URI to start on";
        }
    }
    QJsonDocument Body;
    if ( !URI.isEmpty() )
    {
        Body.setObject( QJsonObject { { "context_uri", URI } } );
    }
    sendRequest( QUrl( Destination ), QNetworkAccessManager::PutOperation, Body );

    // Assume that we have started playing unless we are told otherwise.
    m_bIsPlaying = true;
    emit isPlayingChanged();
    m_ActionSubmissionTimer.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::pause()
{
    QUrl Destination( QString( "%1/pause" ).arg( PLAYER_BASE_URL ) );
    sendRequest( Destination, QNetworkAccessManager::PutOperation );

    // Assume that we have paused unless we are told otherwise.
    m_bIsPlaying = false;
    emit isPlayingChanged();
    m_ActionSubmissionTimer.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::previous()
{
    QUrl Destination( QString( "%1/previous" ).arg( PLAYER_BASE_URL ) );
    sendRequest( Destination, QNetworkAccessManager::PostOperation );

    // Reset track progress.
    m_iTrackPosition = 0;
    emit trackPositionChanged();
    m_iTrackDuration = 0;
    emit trackDurationChanged();
    m_ActionSubmissionTimer.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::next()
{
    QUrl Destination( QString( "%1/next" ).arg( PLAYER_BASE_URL ) );
    sendRequest( Destination, QNetworkAccessManager::PostOperation );

    // Assume the track is at the beginning.
    m_iTrackPosition = 0;
    emit trackPositionChanged();
    m_iTrackDuration = 0;
    emit trackDurationChanged();
    m_ActionSubmissionTimer.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::seek( const int iPosition )
{
    QUrl Destination( QString( "%1/seek?position_ms=%2" ).arg( PLAYER_BASE_URL ).arg( iPosition * 1000 ) );
    sendRequest( Destination, QNetworkAccessManager::PutOperation );

    // Assume the seek request will be accepted.
    m_iTrackPosition = iPosition;
    emit trackPositionChanged();
    m_ActionSubmissionTimer.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::enableShuffle( bool bValue )
{
    QUrl Destination( QString( "%1/shuffle?state=%2" ).arg( PLAYER_BASE_URL, bValue ? "true" : "false" ) );
    sendRequest( Destination, QNetworkAccessManager::PutOperation );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::enableRepeat( bool bValue, bool bAll )
{
    QString State = ( bValue ? ( bAll ? "context" : "track" ) : "off" );
    QUrl Destination( QString( "%1/repeat?state=%2" ).arg( PLAYER_BASE_URL, State ) );
    sendRequest( Destination, QNetworkAccessManager::PutOperation );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::commandDeviceVolume( int iValue )
{
    QUrl Destination( QString( "%1/volume?volume_percent=%2" ).arg( PLAYER_BASE_URL ).arg( iValue ) );
    sendRequest( Destination, QNetworkAccessManager::PutOperation );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::search( const QString & Query )
{
    if ( !Query.isEmpty() )
    {
        QUrl Destination( QString( "https://api.spotify.com/v1/search?type=track&market=%1&limit=20&q=%2" )
                              .arg( m_Market, QUrl::toPercentEncoding( Query ) ) );
        sendRequest( Destination );
    }
    else if ( !m_SearchResults.isEmpty() )
    {
        m_SearchResults.clear();
        emit searchResultsChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::queue( const QString & URI )
{
    QUrl Destination( QString( "%1/queue?uri=%2" ).arg( PLAYER_BASE_URL, URI ) );
    sendRequest( Destination, QNetworkAccessManager::PostOperation );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::transfer( const QString & DeviceID )
{
    QJsonDocument Body( QJsonObject { { "device_ids", QJsonArray { DeviceID } } } );
    sendRequest( QUrl( PLAYER_BASE_URL ), QNetworkAccessManager::PutOperation, Body );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCSpotify::formatDuration( const int iValue )
{
    int iSeconds = iValue % 60;
    QString Seconds = QString::number( iSeconds );
    if ( 10 > iSeconds )
    {
        Seconds = Seconds.rightJustified( 2, QChar( '0' ) );
    }
    return QString( "%1:%2" ).arg( iValue / 60 ).arg( Seconds );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body )
{
    if ( this == pSender )
    {
        if ( 200 == iStatusCode )
        {
            if ( Body.isObject() )
            {
                QJsonObject ResponseObject = Body.object();

                // First, check if this reply contains an updated access token.
                if ( ResponseObject.contains( "access_token" ) )
                {
                    m_AccessToken = ResponseObject.value( "access_token" ).toString();
                    m_AccessTokenType = ResponseObject.value( "token_type" ).toString();

                    // Ensure the refresh timer interval is still sufficient.
                    int iExpiresIn = ResponseObject.value( "expires_in" ).toInt();
                    int iInterval = ( iExpiresIn - 60 ) * 1000;
                    if ( ( 0 < iInterval ) && ( m_AccessTokenRefreshTimer.interval() > iInterval ) )
                    {
                        // Bump the interval.
                        m_AccessTokenRefreshTimer.setInterval( iInterval );
                    }
                    else
                    {
                        // Inside bounds, leave it alone.
                    }

                    // Update the authorization header to use in requests.
                    m_AccessTokenAuthorization = QString( "%1 %2" ).arg( m_AccessTokenType, m_AccessToken ).toUtf8();

                    // Start/restart timers since we just got a fresh access token.
                    m_UpdateTimer.start();
                    m_AccessTokenRefreshTimer.start();

                    // Refresh data right away in case a previous request was rejected.
                    refresh();
                    refreshUserProfile();
                    refreshPlaylists();
                    refreshDevices();

                    qDebug() << "Received new access token from Spotify, asking again in "
                             << ( m_AccessTokenRefreshTimer.interval() / 1000 ) << " seconds";
                }
                else if ( ResponseObject.contains( "is_playing" ) )
                {
                    // Playback information.
                    // Kick the inactivity timer.
                    m_InactivityTimer.start();

                    // Indicate an active state.
                    if ( !m_bIsActive )
                    {
                        m_bIsActive = true;
                        emit isActiveChanged();
                    }

                    bool bIsPlaying = ResponseObject.value( "is_playing" ).toBool();
                    if ( ( m_bIsPlaying != bIsPlaying ) && ( !m_ActionSubmissionTimer.isActive() ) )
                    {
                        m_bIsPlaying = bIsPlaying;
                        emit isPlayingChanged();
                    }
                    if ( ResponseObject.contains( "device" ) )
                    {
                        QJsonObject DeviceObject = ResponseObject.value( "device" ).toObject();
                        if ( DeviceObject.contains( "name" ) )
                        {
                            QString DeviceName = DeviceObject.value( "name" ).toString();
                            if ( m_DeviceName != DeviceName )
                            {
                                m_DeviceName = DeviceName;
                                emit deviceNameChanged();
                            }
                        }
                        if ( DeviceObject.contains( "type" ) )
                        {
                            QString DeviceType = DeviceObject.value( "type" ).toString();
                            if ( m_DeviceType != DeviceType )
                            {
                                m_DeviceType = DeviceType;
                                emit deviceTypeChanged();
                            }
                        }
                        if ( DeviceObject.contains( "volume_percent" )
                             && ( !DeviceObject.value( "volume_percent" ).isNull() ) )
                        {
                            int iVolume = DeviceObject.value( "volume_percent" ).toInt();
                            if ( m_iDeviceVolume != iVolume )
                            {
                                m_iDeviceVolume = iVolume;
                                emit deviceVolumeChanged();
                            }
                        }
                    }
                    if ( ResponseObject.contains( "shuffle_state" ) )
                    {
                        bool bShuffleEnabled = ResponseObject.value( "shuffle_state" ).toBool();
                        if ( m_bShuffleEnabled != bShuffleEnabled )
                        {
                            m_bShuffleEnabled = bShuffleEnabled;
                            emit shuffleEnabledChanged();
                        }
                    }
                    if ( ResponseObject.contains( "repeat_state" ) )
                    {
                        QString RepeatState = ResponseObject.value( "repeat_state" ).toString();
                        bool bRepeatOneEnabled = ( "track" == RepeatState );
                        bool bRepeatAllEnabled = ( "context" == RepeatState );
                        if ( m_bRepeatOneEnabled != bRepeatOneEnabled )
                        {
                            m_bRepeatOneEnabled = bRepeatOneEnabled;
                            emit repeatOneEnabledChanged();
                        }
                        if ( m_bRepeatAllEnabled != bRepeatAllEnabled )
                        {
                            m_bRepeatAllEnabled = bRepeatAllEnabled;
                            emit repeatAllEnabledChanged();
                        }
                    }
                    if ( ResponseObject.contains( "progress_ms" ) )
                    {
                        int iTrackPosition = ResponseObject.value( "progress_ms" ).toInt() / 1000;
                        if ( ( m_iTrackPosition != iTrackPosition ) && ( !m_ActionSubmissionTimer.isActive() ) )
                        {
                            m_iTrackPosition = iTrackPosition;
                            emit trackPositionChanged();
                        }
                    }
                    if ( ResponseObject.contains( "context" ) )
                    {
                        QJsonObject ContextObject = ResponseObject.value( "context" ).toObject();
                        if ( ContextObject.contains( "type" ) && ContextObject.contains( "uri" ) )
                        {
                            QString ContextType = ContextObject.value( "type" ).toString();
                            if ( "playlist" == ContextType )
                            {
                                QString URI = ContextObject.value( "uri" ).toString();
                                QString PlaylistID = URI.split( QChar( ':' ) ).last();

                                // Request the name of the playlist.
                                QUrl Destination( QString( "https://api.spotify.com/v1/playlists/%1?fields=name,uri" )
                                                      .arg( PlaylistID ) );
                                sendRequest( Destination );
                            }
                            else if ( !m_PlaylistName.isEmpty() )
                            {
                                // Clear stale context.
                                m_PlaylistName.clear();
                                emit playlistNameChanged();
                            }
                        }
                        else if ( ContextObject.isEmpty() )
                        {
                            m_PlaylistName.clear();
                            emit playlistNameChanged();
                        }
                    }
                    if ( ResponseObject.contains( "item" ) )
                    {
                        QJsonObject ItemObject = ResponseObject.value( "item" ).toObject();
                        if ( ItemObject.contains( "name" ) )
                        {
                            QString TrackName = ItemObject.value( "name" ).toString();
                            if ( m_TrackName != TrackName )
                            {
                                m_TrackName = TrackName;
                                emit trackNameChanged();
                            }
                        }
                        if ( ItemObject.contains( "duration_ms" ) )
                        {
                            int iTrackDuration = ItemObject.value( "duration_ms" ).toInt() / 1000;
                            if ( ( m_iTrackDuration != iTrackDuration ) && ( !m_ActionSubmissionTimer.isActive() ) )
                            {
                                m_iTrackDuration = iTrackDuration;
                                emit trackDurationChanged();
                            }
                        }
                        if ( ItemObject.contains( "album" ) )
                        {
                            QJsonObject AlbumObject = ItemObject.value( "album" ).toObject();
                            if ( AlbumObject.contains( "name" ) )
                            {
                                QString TrackAlbum = AlbumObject.value( "name" ).toString();
                                if ( m_TrackAlbum != TrackAlbum )
                                {
                                    m_TrackAlbum = TrackAlbum;
                                    emit trackAlbumChanged();
                                }
                            }
                            if ( AlbumObject.contains( "images" ) )
                            {
                                // Take the first image, which is the highest resolution.
                                QJsonArray AlbumImagesArray = AlbumObject.value( "images" ).toArray();
                                if ( !AlbumImagesArray.isEmpty() )
                                {
                                    QJsonObject AlbumImageObject = AlbumImagesArray.first().toObject();
                                    if ( AlbumImageObject.contains( "url" ) )
                                    {
                                        QUrl TrackAlbumArt( AlbumImageObject.value( "url" ).toString() );
                                        if ( m_TrackAlbumArt != TrackAlbumArt )
                                        {
                                            m_TrackAlbumArt = TrackAlbumArt;
                                            emit trackAlbumArtChanged();
                                        }
                                    }
                                }
                                else if ( !m_TrackAlbumArt.isEmpty() )
                                {
                                    // No artwork for the track, clear any previous value.
                                    m_TrackAlbumArt.clear();
                                    emit trackAlbumArtChanged();
                                }
                            }
                        }
                        if ( ItemObject.contains( "artists" ) )
                        {
                            const QJsonArray ArtistsArray = ItemObject.value( "artists" ).toArray();
                            if ( !ArtistsArray.isEmpty() )
                            {
                                QString TrackArtist;
                                for ( const auto & Artist : ArtistsArray )
                                {
                                    QJsonObject ArtistObject = Artist.toObject();
                                    if ( ArtistObject.contains( "name" ) )
                                    {
                                        QString ArtistName = ArtistObject.value( "name" ).toString();
                                        if ( !ArtistName.isEmpty() )
                                        {
                                            if ( !TrackArtist.isEmpty() )
                                            {
                                                TrackArtist.append( ", " );
                                            }
                                            TrackArtist.append( ArtistName );
                                        }
                                    }
                                }
                                if ( ( !TrackArtist.isEmpty() ) && ( m_TrackArtist != TrackArtist ) )
                                {
                                    m_TrackArtist = TrackArtist;
                                    emit trackArtistChanged();
                                }
                            }
                        }
                    }
                }
                else if ( ( 2 == ResponseObject.size() ) && ResponseObject.contains( "name" )
                          && ResponseObject.contains( "uri" ) )
                {
                    // Playlist name.
                    QString PlaylistName = ResponseObject.value( "name" ).toString();
                    if ( m_PlaylistName != PlaylistName )
                    {
                        m_PlaylistName = PlaylistName;
                        emit playlistNameChanged();
                    }
                }
                else if ( ResponseObject.contains( "tracks" ) )
                {
                    // Search results.
                    QVariantList SearchResultsModel;
                    QJsonObject TracksObject = ResponseObject.value( "tracks" ).toObject();
                    if ( TracksObject.contains( "items" ) )
                    {
                        const QJsonArray ItemsArray = TracksObject.value( "items" ).toArray();
                        for ( const auto & Item : ItemsArray )
                        {
                            QJsonObject ItemObject = Item.toObject();
                            if ( !ItemObject.isEmpty() )
                            {
                                QVariantMap SearchResult { { "name", ItemObject.value( "name" ).toString() },
                                                           { "uri", ItemObject.value( "uri" ).toString() } };
                                if ( ItemObject.contains( "artists" ) )
                                {
                                    const QJsonArray ArtistsArray = ItemObject.value( "artists" ).toArray();
                                    if ( !ArtistsArray.isEmpty() )
                                    {
                                        QString TrackArtist;
                                        for ( const auto & Artist : ArtistsArray )
                                        {
                                            QJsonObject ArtistObject = Artist.toObject();
                                            if ( ArtistObject.contains( "name" ) )
                                            {
                                                QString ArtistName = ArtistObject.value( "name" ).toString();
                                                if ( !ArtistName.isEmpty() )
                                                {
                                                    if ( !TrackArtist.isEmpty() )
                                                    {
                                                        TrackArtist.append( ", " );
                                                    }
                                                    TrackArtist.append( ArtistName );
                                                }
                                            }
                                        }
                                        SearchResult[ "artist" ] = TrackArtist;
                                    }
                                }
                                if ( ItemObject.contains( "album" ) )
                                {
                                    QJsonObject AlbumObject = ItemObject.value( "album" ).toObject();
                                    if ( AlbumObject.contains( "name" ) )
                                    {
                                        SearchResult[ "album" ] = AlbumObject.value( "name" ).toString();
                                    }
                                    if ( AlbumObject.contains( "images" ) )
                                    {
                                        // Take the first image, which is the highest resolution.
                                        QJsonArray AlbumImagesArray = AlbumObject.value( "images" ).toArray();
                                        if ( !AlbumImagesArray.isEmpty() )
                                        {
                                            QJsonObject AlbumImageObject = AlbumImagesArray.first().toObject();
                                            if ( AlbumImageObject.contains( "url" ) )
                                            {
                                                SearchResult[ "image" ] =
                                                    QUrl( AlbumImageObject.value( "url" ).toString() );
                                            }
                                        }
                                    }
                                }

                                SearchResultsModel.append( SearchResult );
                            }
                        }
                    }

                    if ( m_SearchResults != SearchResultsModel )
                    {
                        m_SearchResults = SearchResultsModel;
                        emit searchResultsChanged();
                    }
                }
                else if ( ResponseObject.contains( "items" ) )
                {
                    // Playlists information.
                    QVariantList PlaylistsModel;
                    const QJsonArray PlaylistsArray = ResponseObject.value( "items" ).toArray();
                    for ( const auto & Playlist : PlaylistsArray )
                    {
                        QJsonObject PlaylistObject = Playlist.toObject();
                        if ( !PlaylistObject.isEmpty() )
                        {
                            QVariantMap PlaylistItem { { "name", PlaylistObject.value( "name" ).toString() },
                                                       { "uri", PlaylistObject.value( "uri" ).toString() },
                                                       { "isPublic", PlaylistObject.value( "public" ).toBool() } };
                            if ( PlaylistObject.contains( "tracks" ) )
                            {
                                QJsonObject TracksObject = PlaylistObject.value( "tracks" ).toObject();
                                PlaylistItem[ "trackCount" ] = TracksObject.value( "total" ).toInt();
                            }
                            if ( PlaylistObject.contains( "images" ) )
                            {
                                // Take the first image, which is the highest resolution.
                                QJsonArray ImagesArray = PlaylistObject.value( "images" ).toArray();
                                if ( !ImagesArray.isEmpty() )
                                {
                                    QJsonObject ImageObject = ImagesArray.first().toObject();
                                    if ( ImageObject.contains( "url" ) )
                                    {
                                        PlaylistItem[ "image" ] = QUrl( ImageObject.value( "url" ).toString() );
                                    }
                                }
                            }

                            PlaylistsModel.append( PlaylistItem );
                        }
                    }

                    if ( m_Playlists != PlaylistsModel )
                    {
                        m_Playlists = PlaylistsModel;
                        emit playlistsChanged();
                    }
                }
                else if ( ResponseObject.contains( "devices" ) )
                {
                    // Devices information.
                    QVariantList DevicesModel;
                    const QJsonArray DevicesArray = ResponseObject.value( "devices" ).toArray();
                    for ( const auto & Device : DevicesArray )
                    {
                        QJsonObject DeviceObject = Device.toObject();
                        if ( !DeviceObject.isEmpty() )
                        {
                            QString Name = DeviceObject.value( "name" ).toString();
                            QString ID = DeviceObject.value( "id" ).toString();

                            // Is this the preferred device?
                            if ( m_PreferredDevice == Name )
                            {
                                // Yes, store its ID.
                                if ( m_PreferredDeviceID != ID )
                                {
                                    m_PreferredDeviceID = ID;
                                    qDebug() << "Received ID of preferred Spotify device: " << m_PreferredDevice;
                                }
                            }

                            QVariantMap PlaylistItem { { "name", Name }, { "id", ID } };
                            DevicesModel.append( PlaylistItem );
                        }
                    }

                    if ( !DevicesModel.isEmpty() )
                    {
                        // Arrange alphabetically.
                        std::sort( DevicesModel.begin(),
                                   DevicesModel.end(),
                                   [=]( const QVariant & Left, const QVariant & Right ) {
                                       return Left.toMap().value( "name" ).toString()
                                              < Right.toMap().value( "name" ).toString();
                                   } );
                    }

                    if ( m_Devices != DevicesModel )
                    {
                        m_Devices = DevicesModel;
                        emit devicesChanged();
                    }
                }
                else
                {
                    // Assume this is user profile information.
                    if ( ResponseObject.contains( "display_name" ) )
                    {
                        QString UserName = ResponseObject.value( "display_name" ).toString();
                        if ( m_UserName != UserName )
                        {
                            m_UserName = UserName;
                            emit userNameChanged();
                        }
                    }
                    if ( ResponseObject.contains( "email" ) )
                    {
                        QString Email = ResponseObject.value( "email" ).toString();
                        if ( m_UserEmail != Email )
                        {
                            m_UserEmail = Email;
                            emit userEmailChanged();
                        }
                    }
                    if ( ResponseObject.contains( "product" ) )
                    {
                        QString Subscription = ResponseObject.value( "product" ).toString();
                        if ( m_UserSubscription != Subscription )
                        {
                            m_UserSubscription = Subscription;
                            emit userSubscriptionChanged();
                        }
                    }
                    if ( ResponseObject.contains( "images" ) )
                    {
                        // Only be concerned with the first image.
                        QJsonArray ImagesArray = ResponseObject.value( "images" ).toArray();
                        if ( !ImagesArray.isEmpty() )
                        {
                            QJsonObject ImageObject = ImagesArray.first().toObject();
                            if ( ImageObject.contains( "url" ) )
                            {
                                QUrl UserImage( ImageObject.value( "url" ).toString() );
                                if ( m_UserImage != UserImage )
                                {
                                    m_UserImage = UserImage;
                                    emit userImageChanged();
                                }
                            }
                        }
                    }
                }

            }
            else
            {
                qDebug() << "Failed to parse reply from Spotify";
            }
        }
        else if ( 204 == iStatusCode )
        {
            // Success with no content in the response, ignore.
        }
        else if ( 401 == iStatusCode )
        {
            // Access token expired, invalidate the current one, stop making requests, and ask for a new one.
            qDebug() << "Spotify access token expired, so requesting a new one";
            m_AccessTokenAuthorization.clear();
            m_UpdateTimer.stop();
            m_AccessTokenRefreshTimer.stop();
            refreshAccessToken();
        }
        else
        {
            qDebug() << "Ignoring unsuccessful reply from Spotify with status code: " << iStatusCode;
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refreshAccessToken()
{
    if ( ( !m_ClientID.isEmpty() ) && ( !m_ClientSecret.isEmpty() ) && ( !m_RefreshToken.isEmpty() ) )
    {
        qDebug() << "Refreshing Spotify access token";
        static QUrl Destination( "https://accounts.spotify.com/api/token" );
        QUrlQuery Query { { "grant_type", "refresh_token" }, { "refresh_token", m_RefreshToken } };
        QByteArray ClientInfo = QString( "%1:%2" ).arg( m_ClientID, m_ClientSecret ).toUtf8().toBase64();
        QByteArray Authorization = ClientInfo.prepend( "Basic " );
        NetworkInterface::instance()->sendRequest( Destination,
                                                   this,
                                                   QNetworkAccessManager::PostOperation,
                                                   Query.toString( QUrl::FullyEncoded ).toUtf8(),
                                                   "application/x-www-form-urlencoded",
                                                   Authorization );
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::sendRequest( const QUrl & Destination,
                             const QNetworkAccessManager::Operation eRequestType,
                             const QJsonDocument & Body )
{
    NetworkInterface::instance()->sendJSONRequest( Destination, this, eRequestType, Body, m_AccessTokenAuthorization );
}
/*--------------------------------------------------------------------------------------------------------------------*/
