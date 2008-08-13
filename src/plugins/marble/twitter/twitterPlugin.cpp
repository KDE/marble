//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Shashank Singh <shashank.personal@gmail.com>"
//
/** Note :lattitudes are 0° at the equator (low latitude) to 90° at the poles (90° N for the North Pole or 90° S for the South Pole AND "-" For South ; "+" for North
likewise for
longitudes are from 0° at the prime meridian to +180° eastward and ?180° westward
use "E" for "+"
and "W" for "-"
 This piece of information is written here for refrence in case , coder often gets confused about lat Lon like me :P

Right now this plugin displays public twit from Twitter , and gecocodes [i.e getting lat lon from a given street address] using Google Map API , i plan to extend it to use OSM GeoCoding in net few days :) [The API key has been taken for  my personal website , please don't misuse it :) ]
*/

#include "twitterPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QSize>
#include <QRegExp>

namespace Marble
{

twitterPlugin::~twitterPlugin()
{
    delete m_storagePolicy;
}

QStringList twitterPlugin::backendTypes() const
{
    return QStringList("twitter");
}

QString twitterPlugin::renderPolicy() const
{
    return QString("ALWAYS");
}

QStringList twitterPlugin::renderPosition() const
{
    return QStringList("ALWAYS_ON_TOP");
}

QString twitterPlugin::name() const
{
    return tr("twitter ");
}

QString twitterPlugin::guiString() const
{
    return tr("&twitter");
}

QString twitterPlugin::nameId() const
{
    return QString("twitter");
}

QString twitterPlugin::description() const
{
    return tr("show public twitts in their places");
}

QIcon twitterPlugin::icon() const
{
    return QIcon();
}


void twitterPlugin::initialize()
{
    privateFlagForRenderingTwitts = 0;
    m_storagePolicy = new CacheStoragePolicy(MarbleDirs::localPath() + "/cache/");
    m_downloadManager = new HttpDownloadManager(QUrl("http://twiter.com"), m_storagePolicy);
    downloadtwitter(0, 0, 0.0, 0.0, 0.0, 0.0);
//     qDebug() << "twitter plugin was started";



}

bool twitterPlugin::isInitialized() const
{
    return true;
}

bool twitterPlugin::render(GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer)
{
    QBrush brush(QColor(99, 198, 198, 80));
    painter->setPen(QColor(198, 99, 99, 255));
    brush.setColor(QColor(255, 255, 255, 200));
    brush.setStyle(Qt::SolidPattern);
    painter->setBrush(brush);

    if (privateFlagForRenderingTwitts >= 1) {
        for (int counter = 0;counter < 4;counter++)
            painter->drawAnnotation(twitsWithLocation[counter].location , parsedData[counter].user + " said " + "\n" + parsedData[counter].text , QSize(140, 140));
    } else {
        painter->drawAnnotation(GeoDataCoordinates(0.0, 0.0, 0.0, GeoDataCoordinates::Degree), "Twitts are being Downlaoded @Twitter Plugin");
    }
    return true;
}

void twitterPlugin::slotJsonDownloadComplete(QString relativeUrlString, QString id)
{
// temp.twitter = parsedData[counter].text ;
    parsedData = twitterJsonParser.parseAllObjects(QString::fromUtf8(m_storagePolicy->data(id)), 20);

    disconnect(m_downloadManager, SIGNAL(downloadComplete(QString, QString)), this, SLOT(slotJsonDownloadComplete(QString , QString)));

    connect(m_downloadManager, SIGNAL(downloadComplete(QString, QString)), this, SLOT(slotGeoCodingReplyRecieved(QString , QString)));
    for (int counter = 0;counter < 10;counter++) {
        if (parsedData[counter].location != "null") {
            parsedData[counter].location.replace(QRegExp("[?,:!/\\s]+"), "+");//remove whitespace and replace it with + for query api
            findLatLonOfStreetAddress(parsedData [ counter ].location) ;   //this will set temp
        }
    }
}


void twitterPlugin::downloadtwitter(int rangeFrom , int rangeTo , double east , double west , double north , double south)
{
    m_downloadManager->addJob(QUrl("http://twitter.com/statuses/public_timeline.json"), "twitter", "twitter");

    connect(m_downloadManager, SIGNAL(downloadComplete(QString, QString)), this, SLOT(slotJsonDownloadComplete(QString , QString)));

}


void twitterPlugin::findLatLonOfStreetAddress(QString streetAddress)
{
    m_downloadManager->addJob("http://maps.google.com/maps/geo?q=" + streetAddress + "&output=json&key=ABQIAAAASD_v8YRzG0tBD18730KjmRTxoHoIpYL45xcSRJH0O7cH64DuXRT7rQeRcgCLAhjkteQ8vkWAATM_JQ", streetAddress, streetAddress);
    qDebug() << "twitter added Geo Coding job for " << streetAddress;
}

void twitterPlugin::slotGeoCodingReplyRecieved(QString relativeUrlString, QString id)
{
    static int localCountOfTwitts = 0;
    twitterStructure twitterData;
    googleMapDataStructure geoCodedData;

    geoCodedData = twitterJsonParser.geoCodingAPIparseObject(QString::fromUtf8(m_storagePolicy->data(id))) ;
//     twitterData.twit = "hi" ;
    twitterData.location = GeoDataCoordinates(geoCodedData.lat, geoCodedData.lon, 1.0, GeoDataCoordinates::Degree);
    twitsWithLocation.append(twitterData);
    localCountOfTwitts ++;
// qDebug()<<"twitter count has value == " << count ;
    if (localCountOfTwitts >= 5)
        privateFlagForRenderingTwitts = 1;//1 means unblock
}

}

Q_EXPORT_PLUGIN2(twitterPlugin, Marble::twitterPlugin)

#include "twitterPlugin.moc"
