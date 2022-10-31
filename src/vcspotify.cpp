#include "vcspotify.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>

#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static constexpr const char* PLAYER_BASE_URL = "https://api.spotify.com/v1/me/player";
/*--------------------------------------------------------------------------------------------------------------------*/

VCSpotify::VCSpotify(const QString& name, QObject* parent)
    : VCPlugin(name, parent),
      isActive_(false),
      isPlaying_(false),
      shuffleEnabled_(false),
      repeatOneEnabled_(false),
      repeatAllEnabled_(false),
      trackPosition_(0),
      trackDuration_(0),
      deviceVolume_(0),
      market_(QLocale::system().name().split('_').last()) {
    updateTimer_.setInterval(1000);
    updateTimer_.stop();

    // Handle network responses.
    connect(NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCSpotify::handleNetworkReply);

    // Request the initial access token when we are told what the refresh token is and have the client information.
    connect(this, &VCSpotify::clientIDChanged, this, &VCSpotify::refreshAccessToken);
    connect(this, &VCSpotify::clientSecretChanged, this, &VCSpotify::refreshAccessToken);
    connect(this, &VCSpotify::refreshTokenChanged, this, &VCSpotify::refreshAccessToken);

    // Configure a timer to periodically refresh the access token.
    accessTokenRefreshTimer_.setInterval(59 * 60 * 1000);  // 1 minute less than the standard expiry time
    accessTokenRefreshTimer_.setSingleShot(false);
    connect(&accessTokenRefreshTimer_, &QTimer::timeout, this, &VCSpotify::refreshAccessToken);

    // Configure a timer to set the state as idle if a response is not received.
    inactivityTimer_.setInterval(5 * 1000);
    inactivityTimer_.setSingleShot(true);
    connect(&inactivityTimer_, &QTimer::timeout, this, [=] {
        if (isActive_) {
            isActive_ = false;
            emit isActiveChanged();

            // Clear some of the playback state.
            trackPosition_ = 0;
            emit trackPositionChanged();
            trackDuration_ = 0;
            emit trackDurationChanged();
            playlistName_.clear();
            emit playlistNameChanged();
        }
    });

    // Configure a timer to use as a reference to hold off processing after submitting an action.
    // BDP: This helps with keeping things responsive until the API reports the updated state, after which if there is
    //      still a disagreement, the properties will update as normal.
    actionSubmissionTimer_.setInterval(updateTimer_.interval() / 2);
    actionSubmissionTimer_.setSingleShot(true);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refresh() {
    // Refresh current playback information.
    static QUrl destination(QString("%1?market=%2").arg(PLAYER_BASE_URL, market_));
    sendRequest(destination);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refreshDevices() {
    if (!accessTokenAuthorization_.isEmpty()) {
        static QUrl destination(QString("%1/devices").arg(PLAYER_BASE_URL));
        sendRequest(destination);
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refreshUserProfile() {
    if (!accessTokenAuthorization_.isEmpty()) {
        static QUrl destination("https://api.spotify.com/v1/me");
        sendRequest(destination);
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refreshPlaylists() {
    if (!accessTokenAuthorization_.isEmpty()) {
        static QUrl destination("https://api.spotify.com/v1/me/playlists");
        sendRequest(destination);
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::play(const QString& uri) {
    QString destination = QString("%1/play").arg(PLAYER_BASE_URL);
    if (!isActive_) {
        // Idle, so a device must be specified for playing to start.
        if (!preferredDeviceID_.isEmpty()) {
            qDebug() << "Spotify playback is not active, so defaulting to playing on device: " << preferredDevice_;
            destination.append(QString("?device_id=%1").arg(preferredDeviceID_));
        } else {
            qDebug() << "Spotify playback is not active, but there is no preferred device URI to start on";
        }
    }
    QJsonDocument body;
    if (!uri.isEmpty()) {
        body.setObject(QJsonObject{{"context_uri", uri}});
    }
    sendRequest(QUrl(destination), QNetworkAccessManager::PutOperation, body);

    // Assume that we have started playing unless we are told otherwise.
    isPlaying_ = true;
    emit isPlayingChanged();
    actionSubmissionTimer_.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::pause() {
    QUrl destination(QString("%1/pause").arg(PLAYER_BASE_URL));
    sendRequest(destination, QNetworkAccessManager::PutOperation);

    // Assume that we have paused unless we are told otherwise.
    isPlaying_ = false;
    emit isPlayingChanged();
    actionSubmissionTimer_.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::previous() {
    QUrl destination(QString("%1/previous").arg(PLAYER_BASE_URL));
    sendRequest(destination, QNetworkAccessManager::PostOperation);

    // Reset track progress.
    trackPosition_ = 0;
    emit trackPositionChanged();
    trackDuration_ = 0;
    emit trackDurationChanged();
    actionSubmissionTimer_.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::next() {
    QUrl destination(QString("%1/next").arg(PLAYER_BASE_URL));
    sendRequest(destination, QNetworkAccessManager::PostOperation);

    // Assume the track is at the beginning.
    trackPosition_ = 0;
    emit trackPositionChanged();
    trackDuration_ = 0;
    emit trackDurationChanged();
    actionSubmissionTimer_.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::seek(const int position) {
    QUrl destination(QString("%1/seek?position_ms=%2").arg(PLAYER_BASE_URL).arg(position * 1000));
    sendRequest(destination, QNetworkAccessManager::PutOperation);

    // Assume the seek request will be accepted.
    trackPosition_ = position;
    emit trackPositionChanged();
    actionSubmissionTimer_.start();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::enableShuffle(bool value) {
    QUrl destination(QString("%1/shuffle?state=%2").arg(PLAYER_BASE_URL, value ? "true" : "false"));
    sendRequest(destination, QNetworkAccessManager::PutOperation);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::enableRepeat(bool value, bool all) {
    QString state = (value ? (all ? "context" : "track") : "off");
    QUrl destination(QString("%1/repeat?state=%2").arg(PLAYER_BASE_URL, state));
    sendRequest(destination, QNetworkAccessManager::PutOperation);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::commandDeviceVolume(int value) {
    QUrl destination(QString("%1/volume?volume_percent=%2").arg(PLAYER_BASE_URL).arg(value));
    sendRequest(destination, QNetworkAccessManager::PutOperation);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::search(const QString& query) {
    if (!query.isEmpty()) {
        QUrl destination(QString("https://api.spotify.com/v1/search?type=track&market=%1&limit=20&q=%2")
                             .arg(market_, QUrl::toPercentEncoding(query)));
        sendRequest(destination);
    } else if (!searchResults_.isEmpty()) {
        searchResults_.clear();
        emit searchResultsChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::queue(const QString& uri) {
    QUrl destination(QString("%1/queue?uri=%2").arg(PLAYER_BASE_URL, uri));
    sendRequest(destination, QNetworkAccessManager::PostOperation);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::transfer(const QString& deviceID) {
    QJsonDocument body(QJsonObject{{"device_ids", QJsonArray{deviceID}}});
    sendRequest(QUrl(PLAYER_BASE_URL), QNetworkAccessManager::PutOperation, body);
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCSpotify::formatDuration(const int value) {
    int seconds = value % 60;
    QString secondsDisplay = QString::number(seconds);
    if (10 > seconds) {
        secondsDisplay = secondsDisplay.rightJustified(2, '0');
    }
    return QString("%1:%2").arg(value / 60).arg(secondsDisplay);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::handleNetworkReply(int statusCode, QObject* sender, const QJsonDocument& body) {
    if (this == sender) {
        if (200 == statusCode) {
            if (body.isObject()) {
                QJsonObject responseObject = body.object();

                // First, check if this reply contains an updated access token.
                if (responseObject.contains("access_token")) {
                    accessToken_ = responseObject.value("access_token").toString();
                    accessTokenType_ = responseObject.value("token_type").toString();

                    // Ensure the refresh timer interval is still sufficient.
                    int expiresIn = responseObject.value("expires_in").toInt();
                    int interval = (expiresIn - 60) * 1000;
                    if ((0 < interval) && (accessTokenRefreshTimer_.interval() > interval)) {
                        // Bump the interval.
                        accessTokenRefreshTimer_.setInterval(interval);
                    } else {
                        // Inside bounds, leave it alone.
                    }

                    // Update the authorization header to use in requests.
                    accessTokenAuthorization_ = QString("%1 %2").arg(accessTokenType_, accessToken_).toUtf8();

                    // Start/restart timers since we just got a fresh access token.
                    updateTimer_.start();
                    accessTokenRefreshTimer_.start();

                    // Refresh data right away in case a previous request was rejected.
                    refresh();
                    refreshUserProfile();
                    refreshPlaylists();
                    refreshDevices();

                    qDebug() << "Received new access token from Spotify, asking again in "
                             << (accessTokenRefreshTimer_.interval() / 1000) << " seconds";
                } else if (responseObject.contains("is_playing")) {
                    // Playback information.
                    // Kick the inactivity timer.
                    inactivityTimer_.start();

                    // Indicate an active state.
                    if (!isActive_) {
                        isActive_ = true;
                        emit isActiveChanged();
                    }

                    bool isPlaying = responseObject.value("is_playing").toBool();
                    if ((isPlaying_ != isPlaying) && !actionSubmissionTimer_.isActive()) {
                        isPlaying_ = isPlaying;
                        emit isPlayingChanged();
                    }
                    if (responseObject.contains("device")) {
                        QJsonObject deviceObject = responseObject.value("device").toObject();
                        if (deviceObject.contains("name")) {
                            QString deviceName = deviceObject.value("name").toString();
                            if (deviceName_ != deviceName) {
                                deviceName_ = deviceName;
                                emit deviceNameChanged();
                            }
                        }
                        if (deviceObject.contains("type")) {
                            QString deviceType = deviceObject.value("type").toString();
                            if (deviceType_ != deviceType) {
                                deviceType_ = deviceType;
                                emit deviceTypeChanged();
                            }
                        }
                        if (deviceObject.contains("volume_percent") && !deviceObject.value("volume_percent").isNull()) {
                            int volume = deviceObject.value("volume_percent").toInt();
                            if (deviceVolume_ != volume) {
                                deviceVolume_ = volume;
                                emit deviceVolumeChanged();
                            }
                        }
                    }
                    if (responseObject.contains("shuffle_state")) {
                        bool shuffleEnabled = responseObject.value("shuffle_state").toBool();
                        if (shuffleEnabled_ != shuffleEnabled) {
                            shuffleEnabled_ = shuffleEnabled;
                            emit shuffleEnabledChanged();
                        }
                    }
                    if (responseObject.contains("repeat_state")) {
                        QString repeatState = responseObject.value("repeat_state").toString();
                        bool repeatOneEnabled = ("track" == repeatState);
                        bool repeatAllEnabled = ("context" == repeatState);
                        if (repeatOneEnabled_ != repeatOneEnabled) {
                            repeatOneEnabled_ = repeatOneEnabled;
                            emit repeatOneEnabledChanged();
                        }
                        if (repeatAllEnabled_ != repeatAllEnabled) {
                            repeatAllEnabled_ = repeatAllEnabled;
                            emit repeatAllEnabledChanged();
                        }
                    }
                    if (responseObject.contains("progress_ms")) {
                        int trackPosition = responseObject.value("progress_ms").toInt() / 1000;
                        if ((trackPosition_ != trackPosition) && !actionSubmissionTimer_.isActive()) {
                            trackPosition_ = trackPosition;
                            emit trackPositionChanged();
                        }
                    }
                    if (responseObject.contains("context")) {
                        QJsonObject contextObject = responseObject.value("context").toObject();
                        if (contextObject.contains("type") && contextObject.contains("uri")) {
                            QString contextType = contextObject.value("type").toString();
                            if ("playlist" == contextType) {
                                QString uri = contextObject.value("uri").toString();
                                QString playlistID = uri.split(':').last();

                                // Request the name of the playlist.
                                QUrl destination(
                                    QString("https://api.spotify.com/v1/playlists/%1?fields=name,uri").arg(playlistID));
                                sendRequest(destination);
                            } else if (!playlistName_.isEmpty()) {
                                // Clear stale context.
                                playlistName_.clear();
                                emit playlistNameChanged();
                            }
                        } else if (contextObject.isEmpty()) {
                            playlistName_.clear();
                            emit playlistNameChanged();
                        }
                    }
                    if (responseObject.contains("item")) {
                        QJsonObject itemObject = responseObject.value("item").toObject();
                        if (itemObject.contains("name")) {
                            QString trackName = itemObject.value("name").toString();
                            if (trackName_ != trackName) {
                                trackName_ = trackName;
                                emit trackNameChanged();
                            }
                        }
                        if (itemObject.contains("duration_ms")) {
                            int trackDuration = itemObject.value("duration_ms").toInt() / 1000;
                            if ((trackDuration_ != trackDuration) && !actionSubmissionTimer_.isActive()) {
                                trackDuration_ = trackDuration;
                                emit trackDurationChanged();
                            }
                        }
                        if (itemObject.contains("album")) {
                            QJsonObject albumObject = itemObject.value("album").toObject();
                            if (albumObject.contains("name")) {
                                QString trackAlbum = albumObject.value("name").toString();
                                if (trackAlbum_ != trackAlbum) {
                                    trackAlbum_ = trackAlbum;
                                    emit trackAlbumChanged();
                                }
                            }
                            if (albumObject.contains("images")) {
                                // Take the first image, which is the highest resolution.
                                QJsonArray albumImagesArray = albumObject.value("images").toArray();
                                if (!albumImagesArray.isEmpty()) {
                                    QJsonObject albumImageObject = albumImagesArray.first().toObject();
                                    if (albumImageObject.contains("url")) {
                                        QUrl trackAlbumArt(albumImageObject.value("url").toString());
                                        if (trackAlbumArt_ != trackAlbumArt) {
                                            trackAlbumArt_ = trackAlbumArt;
                                            emit trackAlbumArtChanged();
                                        }
                                    }
                                } else if (!trackAlbumArt_.isEmpty()) {
                                    // No artwork for the track, clear any previous value.
                                    trackAlbumArt_.clear();
                                    emit trackAlbumArtChanged();
                                }
                            }
                        }
                        if (itemObject.contains("artists")) {
                            const QJsonArray artistsArray = itemObject.value("artists").toArray();
                            if (!artistsArray.isEmpty()) {
                                QString trackArtist;
                                for (const auto& artist : artistsArray) {
                                    QJsonObject artistObject = artist.toObject();
                                    if (artistObject.contains("name")) {
                                        QString artistName = artistObject.value("name").toString();
                                        if (!artistName.isEmpty()) {
                                            if (!trackArtist.isEmpty()) {
                                                trackArtist.append(", ");
                                            }
                                            trackArtist.append(artistName);
                                        }
                                    }
                                }
                                if (!trackArtist.isEmpty() && (trackArtist_ != trackArtist)) {
                                    trackArtist_ = trackArtist;
                                    emit trackArtistChanged();
                                }
                            }
                        }
                    }
                } else if ((2 == responseObject.size()) && responseObject.contains("name") &&
                           responseObject.contains("uri")) {
                    // Playlist name.
                    QString playlistName = responseObject.value("name").toString();
                    if (playlistName_ != playlistName) {
                        playlistName_ = playlistName;
                        emit playlistNameChanged();
                    }
                } else if (responseObject.contains("tracks")) {
                    // Search results.
                    QVariantList searchResultsModel;
                    QJsonObject tracksObject = responseObject.value("tracks").toObject();
                    if (tracksObject.contains("items")) {
                        const QJsonArray itemsArray = tracksObject.value("items").toArray();
                        for (const auto& item : itemsArray) {
                            QJsonObject itemObject = item.toObject();
                            if (!itemObject.isEmpty()) {
                                QVariantMap searchResult{{"name", itemObject.value("name").toString()},
                                                         {"uri", itemObject.value("uri").toString()}};
                                if (itemObject.contains("artists")) {
                                    const QJsonArray artistsArray = itemObject.value("artists").toArray();
                                    if (!artistsArray.isEmpty()) {
                                        QString trackArtist;
                                        for (const auto& artist : artistsArray) {
                                            QJsonObject artistObject = artist.toObject();
                                            if (artistObject.contains("name")) {
                                                QString artistName = artistObject.value("name").toString();
                                                if (!artistName.isEmpty()) {
                                                    if (!trackArtist.isEmpty()) {
                                                        trackArtist.append(", ");
                                                    }
                                                    trackArtist.append(artistName);
                                                }
                                            }
                                        }
                                        searchResult["artist"] = trackArtist;
                                    }
                                }
                                if (itemObject.contains("album")) {
                                    QJsonObject albumObject = itemObject.value("album").toObject();
                                    if (albumObject.contains("name")) {
                                        searchResult["album"] = albumObject.value("name").toString();
                                    }
                                    if (albumObject.contains("images")) {
                                        // Take the first image, which is the highest resolution.
                                        QJsonArray albumImagesArray = albumObject.value("images").toArray();
                                        if (!albumImagesArray.isEmpty()) {
                                            QJsonObject albumImageObject = albumImagesArray.first().toObject();
                                            if (albumImageObject.contains("url")) {
                                                searchResult["image"] = QUrl(albumImageObject.value("url").toString());
                                            }
                                        }
                                    }
                                }

                                searchResultsModel.append(searchResult);
                            }
                        }
                    }

                    if (searchResults_ != searchResultsModel) {
                        searchResults_ = searchResultsModel;
                        emit searchResultsChanged();
                    }
                } else if (responseObject.contains("items")) {
                    // Playlists information.
                    QVariantList playlistsModel;
                    const QJsonArray playlistsArray = responseObject.value("items").toArray();
                    for (const auto& playlist : playlistsArray) {
                        QJsonObject playlistObject = playlist.toObject();
                        if (!playlistObject.isEmpty()) {
                            QVariantMap playlistItem{{"name", playlistObject.value("name").toString()},
                                                     {"uri", playlistObject.value("uri").toString()},
                                                     {"isPublic", playlistObject.value("public").toBool()}};
                            if (playlistObject.contains("tracks")) {
                                QJsonObject tracksObject = playlistObject.value("tracks").toObject();
                                playlistItem["trackCount"] = tracksObject.value("total").toInt();
                            }
                            if (playlistObject.contains("images")) {
                                // Take the first image, which is the highest resolution.
                                QJsonArray imagesArray = playlistObject.value("images").toArray();
                                if (!imagesArray.isEmpty()) {
                                    QJsonObject imageObject = imagesArray.first().toObject();
                                    if (imageObject.contains("url")) {
                                        playlistItem["image"] = QUrl(imageObject.value("url").toString());
                                    }
                                }
                            }

                            playlistsModel.append(playlistItem);
                        }
                    }

                    if (playlists_ != playlistsModel) {
                        playlists_ = playlistsModel;
                        emit playlistsChanged();
                    }
                } else if (responseObject.contains("devices")) {
                    // Devices information.
                    QVariantList devicesModel;
                    const QJsonArray devicesArray = responseObject.value("devices").toArray();
                    for (const auto& device : devicesArray) {
                        QJsonObject deviceObject = device.toObject();
                        if (!deviceObject.isEmpty()) {
                            QString name = deviceObject.value("name").toString();
                            QString id = deviceObject.value("id").toString();

                            // Is this the preferred device?
                            if (preferredDevice_ == name) {
                                // Yes, store its ID.
                                if (preferredDeviceID_ != id) {
                                    preferredDeviceID_ = id;
                                    qDebug() << "Received ID of preferred Spotify device: " << preferredDevice_;
                                }
                            }

                            QVariantMap playlistItem{{"name", name}, {"id", id}};
                            devicesModel.append(playlistItem);
                        }
                    }

                    if (!devicesModel.isEmpty()) {
                        // Arrange alphabetically.
                        std::sort(
                            devicesModel.begin(), devicesModel.end(), [=](const QVariant& left, const QVariant& right) {
                                return left.toMap().value("name").toString() < right.toMap().value("name").toString();
                            });
                    }

                    if (devices_ != devicesModel) {
                        devices_ = devicesModel;
                        emit devicesChanged();
                    }
                } else {
                    // Assume this is user profile information.
                    if (responseObject.contains("display_name")) {
                        QString userName = responseObject.value("display_name").toString();
                        if (userName_ != userName) {
                            userName_ = userName;
                            emit userNameChanged();
                        }
                    }
                    if (responseObject.contains("email")) {
                        QString email = responseObject.value("email").toString();
                        if (userEmail_ != email) {
                            userEmail_ = email;
                            emit userEmailChanged();
                        }
                    }
                    if (responseObject.contains("product")) {
                        QString subscription = responseObject.value("product").toString();
                        if (userSubscription_ != subscription) {
                            userSubscription_ = subscription;
                            emit userSubscriptionChanged();
                        }
                    }
                    if (responseObject.contains("images")) {
                        // Only be concerned with the first image.
                        QJsonArray imagesArray = responseObject.value("images").toArray();
                        if (!imagesArray.isEmpty()) {
                            QJsonObject imageObject = imagesArray.first().toObject();
                            if (imageObject.contains("url")) {
                                QUrl userImage(imageObject.value("url").toString());
                                if (userImage_ != userImage) {
                                    userImage_ = userImage;
                                    emit userImageChanged();
                                }
                            }
                        }
                    }
                }

            } else {
                qDebug() << "Failed to parse reply from Spotify";
            }
        } else if (204 == statusCode) {
            // Success with no content in the response, ignore.
        } else if (401 == statusCode) {
            // Access token expired, invalidate the current one, stop making requests, and ask for a new one.
            qDebug() << "Spotify access token expired, so requesting a new one";
            accessTokenAuthorization_.clear();
            updateTimer_.stop();
            accessTokenRefreshTimer_.stop();
            refreshAccessToken();
        } else {
            qDebug() << "Ignoring unsuccessful reply from Spotify with status code: " << statusCode;
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::refreshAccessToken() {
    if (!clientID_.isEmpty() && !clientSecret_.isEmpty() && !refreshToken_.isEmpty()) {
        qDebug() << "Refreshing Spotify access token";
        static QUrl destination("https://accounts.spotify.com/api/token");
        QUrlQuery query{{"grant_type", "refresh_token"}, {"refresh_token", refreshToken_}};
        QByteArray clientInfo = QString("%1:%2").arg(clientID_, clientSecret_).toUtf8().toBase64();
        QByteArray authorization = clientInfo.prepend("Basic ");
        NetworkInterface::instance()->sendRequest(destination,
                                                  this,
                                                  QNetworkAccessManager::PostOperation,
                                                  query.toString(QUrl::FullyEncoded).toUtf8(),
                                                  "application/x-www-form-urlencoded",
                                                  authorization);
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCSpotify::sendRequest(const QUrl& destination,
                            const QNetworkAccessManager::Operation requestType,
                            const QJsonDocument& body) {
    NetworkInterface::instance()->sendJSONRequest(destination, this, requestType, body, accessTokenAuthorization_);
}
/*--------------------------------------------------------------------------------------------------------------------*/
