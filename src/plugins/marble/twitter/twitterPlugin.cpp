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
 
*/

#include "twitterPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QSize>

// #include "GeoDataLatLonAltBox.h"
// #include "ViewportParams.h"

QStringList twitterPlugin::backendTypes() const
{
        return QStringList ( "twitter" );
}

QString twitterPlugin::renderPolicy() const
{
        return QString ( "ALWAYS" );
}

QStringList twitterPlugin::renderPosition() const
{
        return QStringList ( "ALWAYS_ON_TOP" );
}

QString twitterPlugin::name() const
{
        return tr ( "twitter " );
}

QString twitterPlugin::guiString() const
{
        return tr ( "&twitter" );
}

QString twitterPlugin::nameId() const
{
        return QString ( "twitter" );
}

QString twitterPlugin::description() const
{
        return tr ( "show public twitts in their places");
}

QIcon twitterPlugin::icon () const
{
        return QIcon();
}


void twitterPlugin::initialize ()
{
        flag = 0;
        m_storagePolicy = new CacheStoragePolicy ( MarbleDirs::localPath() + "/cache/" );
        m_downloadManager = new HttpDownloadManager ( QUrl ( "htttp://twiter.com" ), m_storagePolicy );
        downloadtwitter ( 0, 0, 0.0, 0.0, 0.0, 0.0 );
        qDebug()<<"twitter plugin was started";

}

bool twitterPlugin::isInitialized () const
{
        return true;
}

bool twitterPlugin::render ( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
        QBrush brush( QColor( 99, 198, 198, 80 ) );
        painter->setPen( QColor( 198, 99, 99, 255 ) );
        brush.setColor( QColor( 255, 255, 255, 200 ) );
        brush.setStyle( Qt::SolidPattern );
        painter->setBrush( brush );

        for (int counter=0;counter<5;counter++)
                painter->drawAnnotation (  twitsWithLocation[counter].location , parsedData[counter].user +" said " + "\n" +parsedData[counter].text , QSize(140,140) );

        return true;
}

void twitterPlugin::slotJsonDownloadComplete ( QString relativeUrlString, QString id )
{

        QFile file("/root/marble_latest/src/plugins/marble/twitter/public.json");

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return;

        QTextStream in(&file);
        QString dataFromFile = in.readAll();
        twitterStructure temp;
        parsedData = twitterJsonParser.parseAllObjects(dataFromFile,5);
        for (int counter=0;counter<5;counter++) {

                temp.twitter= parsedData[counter].text ;
                temp.location=findLatLonOfStreetAddress ( parsedData [ counter ].location ) ;
                twitsWithLocation.append ( temp  );
        }
}


void twitterPlugin::downloadtwitter ( int rangeFrom , int rangeTo ,double east ,double west , double north ,double south )
{
//         m_downloadManager->addJob ( QUrl (  ),"twitter","twitter");
        slotJsonDownloadComplete("twitter","twitter");


}

GeoDataPoint twitterPlugin::findLatLonOfStreetAddress(QString streetAddress)
{
        GeoDataPoint tempLatLon(0.0,0.0,0.0,GeoDataPoint::Degree );
        m_downloadManager->addJob("http://maps.google.com/maps/geo?q="+streetAddress+"&output=json&key=ABQIAAAASD_v8YRzG0tBD18730KjmRTxoHoIpYL45xcSRJH0O7cH64DuXRT7rQeRcgCLAhjkteQ8vkWAATM_JQ","finLatLon","findLatLon");
        connect ( m_downloadManager, SIGNAL ( downloadComplete ( QString, QString ) ), this, SLOT ( slotGeoCodingReplyRecieved ( QString , QString ) ) );
        return tempLatLon;
}

void twitterPlugin::slotGeoCodingReplyRecieved(QString ,QString)
{

}
Q_EXPORT_PLUGIN2 ( twitterPlugin, twitterPlugin )

#include "twitterPlugin.moc"
