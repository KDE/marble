//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Shashank Singh <shashank.personal@gmail.com>
//

//
// This class is a test plugin.
//

#ifndef MARBLETWITTERPLUGIN_H
#define MARBLETWITTERPLUGIN_H
#define RADIANSTODEGREES 57.2957795

#include "../lib/HttpDownloadManager.h"
#include "../lib/CacheStoragePolicy.h"
#include "jsonparser.h"
#include "RenderPlugin.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

/**
 * @short The class that specifies the a simple panormaio plugin
 *
 */
struct twitterStructure
{
    QString twit ;
    GeoDataCoordinates location;
};

class twitterPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.twitterPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    MARBLE_PLUGIN(twitterPlugin)

public:
    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon() const;


    void initialize();

    bool isInitialized() const;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0);

    ~twitterPlugin();

public Q_SLOTS:
    void slotJsonDownloadComplete(QString , QString);   //completed download of json reply fom panoramio
    void slotGeoCodingReplyRecieved(QString , QString);   //completed download of image

Q_SIGNALS:
    void statusMessageForImageDownloadingProcess(const QString&);
private:
    CacheStoragePolicy *m_storagePolicy;
    HttpDownloadManager *m_downloadManager;
    jsonParser twitterJsonParser;
    void downloadtwitter(int, int, qreal, qreal, qreal, qreal);
    QList <twitterStructure > twitsWithLocation;//this list will hold pointers to TWITT we have downloaded successfully figured out :)
    QList <twitterDataStructure> parsedData;
   void findLatLonOfStreetAddress(QString streetAddress);
    int privateFlagForRenderingTwitts;//this flag is one when globe has an Image  (downloaded or already there in cache)

};

}

#endif
