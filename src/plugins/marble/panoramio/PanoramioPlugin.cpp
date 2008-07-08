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


QStringList PanoramioPlugin::backendTypes() const
{
    return QStringList ( "PanoramioJson" );
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
    return tr ( "panoramio" );
}

QString PanoramioPlugin::guiString() const
{
    return tr ( "&Panoramio" );
}

QString PanoramioPlugin::nameId() const
{
    return QString ( "Panoramio-Plugin" );
}

QString PanoramioPlugin::description() const
{
    return tr ("Autoamtically dwonlaods images from aorund the world in preference to their popularity" );
}

QIcon PanoramioPlugin::icon () const
{
    return QIcon();
}


void PanoramioPlugin::initialize ()
{
    flag=0;
    numberOfImagesToShow=5;
    m_storagePolicy=new CacheStoragePolicy ( MarbleDirs::localPath() + "/cache/" );
    m_downloadManager=new HttpDownloadManager ( QUrl ( "htttp://mw2.google.com/" ),m_storagePolicy );
    downloadPanoramio ( 0,numberOfImagesToShow );

}

bool PanoramioPlugin::isInitialized () const
{
    return true;
}

bool PanoramioPlugin::render ( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    if ( flag == 1 )
    {
        for ( int x=0; x< imagesWeHave.count();x++ )
        {
            painter->drawPixmap ( GeoDataPoint ( parsedData[x].longitude,parsedData[x].latitude,/*2.0,3.0,*/0.0 ),imagesWeHave[x],GeoDataPoint::Degree);
            qDebug() <<"Shanky=Coordinates arelon_lat" << parsedData[x].longitude << parsedData[x].latitude;
        }
    }
    return true;
}

void PanoramioPlugin::slotJsonDownloadComplete ( QString relativeUrlString, QString id )
{
    disconnect ( m_downloadManager,SIGNAL ( downloadComplete ( QString, QString ) ),this, SLOT ( slotJsonDownloadComplete ( QString , QString ) ) );
    connect ( m_downloadManager,SIGNAL ( downloadComplete ( QString, QString ) ),this, SLOT ( slotImageDownloadComplete ( QString , QString ) ) );

    for ( int x=0;x<numberOfImagesToShow;x++ )
    {
        temp=panoramioJsonParser.parseObjectOnPosition ( QString::fromUtf8 ( m_storagePolicy->data ( id ) ), x );
        parsedData.append ( temp );
        m_downloadManager->addJob ( QUrl ( temp.photo_file_url ),temp.photo_title, QString::number ( x ) );
        qDebug() <<":::::::shanky1"<<temp.photo_file_url;
    }

//     qDebug() <<"::::::::::::::shanky1"<<parsedData.photo_file_url;

}

void PanoramioPlugin::slotImageDownloadComplete ( const QString relativeUrlString, const QString id )
{

    //     temp.loadFromData ( m_storagePolicy->data ( id ) );
    tempImage.load ( MarbleDirs::localPath() + "/cache/"+relativeUrlString );
    imagesWeHave.append ( tempImage.scaled ( QSize ( 50,50 ),  Qt::KeepAspectRatio , Qt::FastTransformation ) );
    qDebug() <<"::::::::::::::shanky2"<<id<<"="<<tempImage.isNull() <<MarbleDirs::localPath() + "/cache/"+relativeUrlString ;
    flag=1;
}
void PanoramioPlugin::downloadPanoramio ( int rangeFrom , int rangeTo )
{
    m_downloadManager->addJob ( QUrl ( "http://www.panoramio.com/map/get_panoramas.php?from="+QString::number(rangeFrom)+"&to="+QString::number(rangeTo)+"&minx=-180&miny=-90&maxx=180&maxy=90" ),"panoramio","panoramio" );
    connect ( m_downloadManager,SIGNAL ( downloadComplete ( QString, QString ) ),this, SLOT ( slotJsonDownloadComplete ( QString , QString ) ) );
    qDebug() <<"::::::::::::::shanky0";
}


Q_EXPORT_PLUGIN2 ( PanoramioPlugin, PanoramioPlugin )

#include "PanoramioPlugin.moc"
