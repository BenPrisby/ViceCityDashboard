#ifndef VCSPOTIFY_H_
#define VCSPOTIFY_H_

#include "vcplugin.h"
#include "networkinterface.h"

class VCSpotify : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( QString userName            READ userName                                    NOTIFY userNameChanged )
    Q_PROPERTY( QString userEmail           READ userEmail                                   NOTIFY userEmailChanged )
    Q_PROPERTY( QString userSubscription    READ userSubscription                            NOTIFY userSubscriptionChanged )
    Q_PROPERTY( QUrl userImage              READ userImage                                   NOTIFY userImageChanged )
    Q_PROPERTY( bool isActive               READ isActive                                    NOTIFY isActiveChanged )
    Q_PROPERTY( bool isPlaying              READ isPlaying                                   NOTIFY isPlayingChanged )
    Q_PROPERTY( bool shuffleEnabled         READ shuffleEnabled                              NOTIFY shuffleEnabledChanged )
    Q_PROPERTY( bool repeatOneEnabled       READ repeatOneEnabled                            NOTIFY repeatOneEnabledChanged )
    Q_PROPERTY( bool repeatAllEnabled       READ repeatAllEnabled                            NOTIFY repeatAllEnabledChanged )
    Q_PROPERTY( QString trackName           READ trackName                                   NOTIFY trackNameChanged )
    Q_PROPERTY( QString trackArtist         READ trackArtist                                 NOTIFY trackArtistChanged )
    Q_PROPERTY( QString trackAlbum          READ trackAlbum                                  NOTIFY trackAlbumChanged )
    Q_PROPERTY( QUrl trackAlbumArt          READ trackAlbumArt                               NOTIFY trackAlbumArtChanged )
    Q_PROPERTY( int trackPosition           READ trackPosition                               NOTIFY trackPositionChanged )
    Q_PROPERTY( int trackDuration           READ trackDuration                               NOTIFY trackDurationChanged )
    Q_PROPERTY( QString playlistName        READ playlistName                                NOTIFY playlistNameChanged )
    Q_PROPERTY( QVariantList playlists      READ playlists                                   NOTIFY playlistsChanged )
    Q_PROPERTY( QString deviceName          READ deviceName                                  NOTIFY deviceNameChanged )
    Q_PROPERTY( QString deviceType          READ deviceType                                  NOTIFY deviceTypeChanged )
    Q_PROPERTY( int deviceVolume            READ deviceVolume                                NOTIFY deviceVolumeChanged )
    Q_PROPERTY( QVariantList devices        READ devices                                     NOTIFY devicesChanged )
    Q_PROPERTY( QString preferredDevice     READ preferredDevice   MEMBER m_PreferredDevice  NOTIFY preferredDeviceChanged )
    Q_PROPERTY( QVariantList searchResults  READ searchResults                               NOTIFY searchResultsChanged )
    Q_PROPERTY( QString clientID            MEMBER m_ClientID                                NOTIFY clientIDChanged )
    Q_PROPERTY( QString clientSecret        MEMBER m_ClientSecret                            NOTIFY clientSecretChanged )
    Q_PROPERTY( QString refreshToken        MEMBER m_RefreshToken                            NOTIFY refreshTokenChanged )

public:
    explicit VCSpotify( const QString & Name, QObject * pParent = nullptr );

    const QString & userName() const { return m_UserName; }
    const QString & userEmail() const { return m_UserEmail; }
    const QString & userSubscription() const { return m_UserSubscription; }
    const QUrl & userImage() const { return m_UserImage; }
    bool isActive() const { return m_bIsActive; }
    bool isPlaying() const { return m_bIsPlaying; }
    bool shuffleEnabled() const { return m_bShuffleEnabled; }
    bool repeatOneEnabled() const { return m_bRepeatOneEnabled; }
    bool repeatAllEnabled() const { return m_bRepeatAllEnabled; }
    const QString & trackName() const { return m_TrackName; }
    const QString & trackArtist() const { return m_TrackArtist; }
    const QString & trackAlbum() const { return m_TrackAlbum; }
    const QUrl & trackAlbumArt() const { return m_TrackAlbumArt; }
    int trackPosition() const { return m_iTrackPosition; }
    int trackDuration() const { return m_iTrackDuration; }
    const QString & playlistName() const { return m_PlaylistName; }
    const QVariantList & playlists() const { return m_Playlists; }
    const QString & deviceName() const { return m_DeviceName; }
    const QString & deviceType() const { return m_DeviceType; }
    const QVariantList & devices() const { return m_Devices; }
    const QString & preferredDevice() const { return m_PreferredDevice; }
    const QVariantList & searchResults() const { return m_SearchResults; }
    int deviceVolume() const { return m_iDeviceVolume; }

signals:
    void userNameChanged();
    void userEmailChanged();
    void userSubscriptionChanged();
    void userImageChanged();
    void isActiveChanged();
    void isPlayingChanged();
    void shuffleEnabledChanged();
    void repeatOneEnabledChanged();
    void repeatAllEnabledChanged();
    void trackNameChanged();
    void trackArtistChanged();
    void trackAlbumChanged();
    void trackAlbumArtChanged();
    void trackPositionChanged();
    void trackDurationChanged();
    void playlistNameChanged();
    void playlistsChanged();
    void deviceNameChanged();
    void deviceTypeChanged();
    void deviceVolumeChanged();
    void devicesChanged();
    void preferredDeviceChanged();
    void searchResultsChanged();
    void clientIDChanged();
    void clientSecretChanged();
    void refreshTokenChanged();

public slots:
    void refresh() override;

    void play( const QString & URI = QString() );
    void pause();
    void previous();
    void next();
    void seek( int iPosition );
    void enableShuffle( bool bValue );
    void enableRepeat( bool bValue, bool bAll = true );
    void commandDeviceVolume( int iValue );
    void search( const QString & Query );
    void queue( const QString & URI );
    void transfer( const QString & DeviceID );
    void refreshDevices();

    QString formatDuration( int iValue );

private slots:
    void handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body );
    void refreshAccessToken();
    void refreshUserProfile();
    void refreshPlaylists();

private:
    QString m_UserName;
    QString m_UserEmail;
    QString m_UserSubscription;
    QUrl m_UserImage;
    bool m_bIsActive;
    bool m_bIsPlaying;
    bool m_bShuffleEnabled;
    bool m_bRepeatOneEnabled;
    bool m_bRepeatAllEnabled;
    QString m_TrackName;
    QString m_TrackArtist;
    QString m_TrackAlbum;
    QUrl m_TrackAlbumArt;
    int m_iTrackPosition;
    int m_iTrackDuration;
    QString m_PlaylistName;
    QVariantList m_Playlists;
    QString m_DeviceName;
    QString m_DeviceType;
    int m_iDeviceVolume;
    QVariantList m_Devices;
    QString m_PreferredDevice;
    QString m_PreferredDeviceID;
    QVariantList m_SearchResults;
    QString m_ClientID;
    QString m_ClientSecret;
    QString m_RefreshToken;
    QString m_AccessToken;
    QString m_AccessTokenType;

    QString m_Market;
    QByteArray m_AccessTokenAuthorization;
    QTimer m_UserProfileRefreshTimer;
    QTimer m_PlaylistsRefreshTimer;
    QTimer m_DevicesRefreshTimer;
    QTimer m_AccessTokenRefreshTimer;
    QTimer m_InactivityTimer;
    QTimer m_ActionSubmissionTimer;

    void sendRequest( const QUrl & Destination,
                      QNetworkAccessManager::Operation eRequestType = QNetworkAccessManager::GetOperation,
                      const QJsonDocument & Body = QJsonDocument() );

    Q_DISABLE_COPY_MOVE( VCSpotify )
};

#endif  // VCSPOTIFY_H_
