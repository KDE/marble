//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Shashank Singh <shashank.personal@gmail.com>"
//

#include "PanoramioPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QSize>
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataPoint.h"
#include "GeoDataLatLonAltBox.h"
#include "ViewportParams.h"

QStringList PanoramioPlugin::backendTypes() const
{
        return QStringList ( "panoramio" );
}

QString PanoramioPlugin::renderPolicy() const
{
        return QString ( "ALWAYS" );
}

QStringList PanoramioPlugin::renderPosition() const
{
        return QStringList ( "ALWAYS_ON_TOP" );
}

QString PanoramioPlugin::name() const
{
        return tr ( "Panoramio Photos" );
}

QString PanoramioPlugin::guiString() const
{
        return tr ( "&Panoramio" );
}

QString PanoramioPlugin::nameId() const
{
        return QString ( "panoramio" );
}

QString PanoramioPlugin::description() const
{
        return tr ( "Automatically downloads images from around the world in preference to their popularity" );
}

QIcon PanoramioPlugin::icon () const
{
        return QIcon();
}


void PanoramioPlugin::initialize ()
{
        flag = 0;
        numberOfImagesToShow = 5;
        m_storagePolicy = new CacheStoragePolicy ( MarbleDirs::localPath() + "/cache/" );
        m_downloadManager = new HttpDownloadManager ( QUrl ( "htttp://mw2.google.com/" ), m_storagePolicy );
        downloadPanoramio ( 0, numberOfImagesToShow , -180.00 / RADIANSTODEGREES  , -90.00 / RADIANSTODEGREES , 180.00 / RADIANSTODEGREES , 90.00 / RADIANSTODEGREES );

}

bool PanoramioPlugin::isInitialized () const
{
        return true;
}

bool PanoramioPlugin::render ( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
        static double deltaWest = 0 , deltaEast = 0 , deltaSouth = 0 , deltaNorth = 0;
        painter->autoMapQuality();
        GeoDataLatLonAltBox mylatLonAltBox = viewport->viewLatLonAltBox();

        double west = mylatLonAltBox.west();
        double east = mylatLonAltBox.east();
        double south = mylatLonAltBox.south();
        double north = mylatLonAltBox.north();
 if ( ( west - deltaWest ) != 0 || ( east - deltaEast != 0 ) || ( south - deltaSouth != 0 ) || ( north - deltaNorth != 00 ) )
 {
          flag = 0 ;
          qDebug() <<"delta part";
//           disconnect(m_downloadManager,SIGNAL ( downloadComplete ( QString, QString ) ),this, SLOT ( slotJsonDownloadComplete ( QString , QString ) ));
         downloadPanoramio ( 0 , numberOfImagesToShow , west , east , north , south );
        disconnect( m_downloadManager, SIGNAL ( downloadComplete ( QString, QString ) ), this, SLOT ( slotImageDownloadComplete ( QString , QString ) ) );
 }

        if ( flag == 1 ) {
                for ( int x = 0; x < imagesWeHave.count(); ++x ) {
                        painter->drawPixmap ( GeoDataPoint ( parsedData[x].longitude,parsedData[x].latitude, 0.0, GeoDataPoint::Degree ), imagesWeHave[x] );
                        painter->setBrush(QBrush(Qt::NoBrush));
                        painter->setPen(Qt::Dense1Pattern);
                        painter->setPen(Qt::white);
                        painter->drawRect(GeoDataPoint ( parsedData[x].longitude,parsedData[x].latitude, 0.0, GeoDataPoint::Degree ),/*parsedData[x].height , parsedData[x].width*/50,50);
//             qDebug() <<"Shanky: Coordinates are lon-lat: " << parsedData[x].longitude << parsedData[x].latitude;
                }
        }
        qDebug() <<"deltas"<<west - deltaWest<<east - deltaEast<<south - deltaSouth<<north - deltaNorth;

        deltaWest = west;
        deltaEast = east ;
        deltaSouth = south ;
        deltaNorth = north;
        return true;
}

void PanoramioPlugin::slotJsonDownloadComplete ( QString relativeUrlString, QString id )
{
        disconnect ( m_downloadManager, SIGNAL ( downloadComplete ( QString, QString ) ), this, SLOT ( slotJsonDownloadComplete ( QString , QString ) ) );
        connect ( m_downloadManager, SIGNAL ( downloadComplete ( QString, QString ) ), this, SLOT ( slotImageDownloadComplete ( QString , QString ) ) );

        for ( int x = 0; x < numberOfImagesToShow; ++x ) {
                temp = panoramioJsonParser.parseObjectOnPosition ( QString::fromUtf8 ( m_storagePolicy->data ( id ) ), x );
                parsedData.append ( temp );
                if ( !m_storagePolicy->fileExists ( temp.photo_title ) ) {
                        m_downloadManager->addJob ( QUrl ( temp.photo_file_url ), temp.photo_title, QString::number ( x ) );
                        qDebug() <<"skipping "<<temp.photo_title;
                }
                qDebug() <<":::::::shanky1" << temp.photo_file_url;
        }

//         HttpJob *job = new HttpJob ( sourceUrl, destFileName, id );
//         m_downloadManager->removeJob(job);
}

void PanoramioPlugin::slotImageDownloadComplete ( const QString relativeUrlString, const QString id )
{

        //     temp.loadFromData ( m_storagePolicy->data ( id ) );
        tempImage.load ( MarbleDirs::localPath() + "/cache/" + relativeUrlString );
        imagesWeHave.append ( tempImage.scaled ( QSize ( 50, 50 ),  Qt::IgnoreAspectRatio , Qt::SmoothTransformation ) );
        qDebug() <<"::::::::::::::shanky2"<< id << "=" << tempImage.isNull() << MarbleDirs::localPath() + "/cache/" + relativeUrlString ;
        flag = 1;
}

void PanoramioPlugin::downloadPanoramio ( int rangeFrom , int rangeTo ,double east ,double west , double north ,double south )
{
        m_downloadManager->addJob ( QUrl ( "http://www.panoramio.com/map/get_panoramas.php?from="
                                           + QString::number ( rangeFrom )
                                           + "&to=" + QString::number ( rangeTo )
                                           + "&minx=" + QString::number ( east * RADIANSTODEGREES )
                                           + "&miny=" + QString::number ( west * RADIANSTODEGREES )
                                           + "&maxx=" + QString::number ( north * RADIANSTODEGREES )
                                           + "&maxy=" + QString::number ( south * RADIANSTODEGREES ) + "&size=medium" ),"panoramio"+QString::number ( east),"panoramio" +QString::number ( east));
        connect ( m_downloadManager,SIGNAL ( downloadComplete ( QString, QString ) ),this, SLOT ( slotJsonDownloadComplete ( QString , QString ) ) );
        qDebug() <<"::::::::::::::shanky0";
//         job = new HttpJob ( QUrl ( "http://www.panoramio.com/map/get_panoramas.php?from="
//                                    + QString::number ( rangeFrom )
//                                    + "&to=" + QString::number ( rangeTo )
//                                    + "&minx=" + QString::number ( east * RADIANSTODEGREES )
//                                    + "&miny=" + QString::number ( west * RADIANSTODEGREES )
//                                    + "&maxx=" + QString::number ( north * RADIANSTODEGREES )
//                                    + "&maxy=" + QString::number ( south * RADIANSTODEGREES ) + "&size=medium" ), "panoramio","panoramio" );
}


Q_EXPORT_PLUGIN2 ( PanoramioPlugin, PanoramioPlugin )

#include "PanoramioPlugin.moc"
