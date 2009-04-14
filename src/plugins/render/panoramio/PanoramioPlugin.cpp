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
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "ViewportParams.h"



namespace Marble {

QStringList PanoramioPlugin::backendTypes() const {
    return QStringList( "panoramio" );
}

QString PanoramioPlugin::renderPolicy() const {
    return QString( "ALWAYS" );
}

QStringList PanoramioPlugin::renderPosition() const {
    return QStringList( "ALWAYS_ON_TOP" );
}

QString PanoramioPlugin::name() const {
    return tr( "Panoramio Photos" );
}

QString PanoramioPlugin::guiString() const {
    return tr( "&Panoramio" );
}

QString PanoramioPlugin::nameId() const {
    return QString( "panoramio" );
}

QString PanoramioPlugin::description() const {
    return tr( "Automatically downloads images from around the world in preference to their popularity" );
}

QIcon PanoramioPlugin::icon() const {
    return QIcon();
}


void PanoramioPlugin::initialize() {
    flag = 0;
    numberOfImagesToShow = 20;
    m_storagePolicy = new CacheStoragePolicy( MarbleDirs::localPath()
                                              + "/cache/" );
    m_downloadManager = new HttpDownloadManager( QUrl("htttp://mw2.google.com/"),
                                                 m_storagePolicy );
    downloadPanoramio( 0, numberOfImagesToShow,
                       -180.00 / RADIANSTODEGREES, -90.00 / RADIANSTODEGREES, 
                        180.00 / RADIANSTODEGREES,  90.00 / RADIANSTODEGREES );
}

bool PanoramioPlugin::isInitialized() const {
    return true;
}

/**
 *This function render whatever images it has in list
 */
bool PanoramioPlugin::render( GeoPainter *painter, ViewportParams *viewport,
                              const QString& renderPos, GeoSceneLayer * layer )
{
    static qreal deltaWest = 0 , deltaEast = 0 , deltaSouth = 0 , deltaNorth = 0;
    painter->autoMapQuality();
    GeoDataLatLonAltBox mylatLonAltBox = viewport->viewLatLonAltBox();
    if ( flag == 1 ) {
        for ( int x = 0; x < images.size(); ++x ) {
            GeoDataCoordinates coordinates( parsedData[x].longitude,
                                            parsedData[x].latitude,
                                            0.0,
                                            GeoDataCoordinates::Degree );
            painter->drawPixmap( coordinates, *( (images.at( x ) )->returnPointerToImage() ) );
// 	  (images.at(x)).render(painter->device(),QPoint(),QRegion(),QWidget::RenderFlags(QWidget::DrawChildren));//,my attempt at drawing a widget
            painter->setBrush( QBrush( Qt::NoBrush ) );
            painter->setPen( Qt::Dense1Pattern );
            painter->setPen( Qt::white );
            painter->drawRect( coordinates,/*parsedData[x].height , parsedData[x].width*/50, 50 );
//              qDebug() <<"Shanky: Coordinates are lon-lat: " << parsedData[x].longitude << parsedData[x].latitude;

        }
    }
    return true;
}

/**
 *this slot is called after the json has been downlaod
 *[1]this flost calls the json parser
 *[2]add jobs in  http Download manager for each parsed entry in parsed json
 */
void PanoramioPlugin::downloadImages( QString relativeUrlString, QString id ) {
    disconnect( m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ),
                this, SLOT( downloadImages( QString , QString ) ) );
    connect( m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ),
             this, SLOT( appendImageToList( QString , QString ) ) );

    for ( int x = 0; x < numberOfImagesToShow; ++x ) {
        temp = panoramioJsonParser.parseObjectOnPosition( QString::fromUtf8( m_storagePolicy->data( id ) ), x );
        parsedData.append( temp );
//	qDebug()<<temp.photo_file_url;
        if ( !m_storagePolicy->fileExists( temp.photo_title ) ) {
            m_downloadManager->addJob( QUrl( temp.photo_file_url ), temp.photo_title, QString::number( x ) );
            qDebug() << "adding " << temp.photo_title;
        }
    }

}

/**
 * this slot is called once a image has been downloaded by HTTPdownloader
 * [1]It appends the image to a imageWidget
 */
void PanoramioPlugin::appendImageToList( const QString relativeUrlString, const QString id ) {
    tempImage.load( MarbleDirs::localPath() + "/cache/" + relativeUrlString );
//     imagesWeHave.append(tempImage.scaled(QSize(50, 50),  Qt::IgnoreAspectRatio , Qt::FastTransformation));
    images.append( new imageWidget );
    ( images.at( ( images.size() ) - 1 ) )->addImage( tempImage.scaled( QSize( 50, 50 ),
                                                                        Qt::IgnoreAspectRatio,
                                                                        Qt::FastTransformation ) );
    qDebug() << __func__ << id << "=" << tempImage.isNull() << MarbleDirs::localPath() + "/cache/" + relativeUrlString ;
    flag = 1;
}

/**
 * This function add jobs to downlaod manager to fetch json file from panoramio servers
 */
void PanoramioPlugin::downloadPanoramio( int rangeFrom, int rangeTo,
                                         qreal east, qreal west,
                                         qreal north , qreal south )
{
    QUrl jsonUrl( "http://www.panoramio.com/map/get_panoramas.php?from="
                  + QString::number( rangeFrom )
                  + "&to=" + QString::number( rangeTo )
                  + "&minx=" + QString::number( east  * RADIANSTODEGREES )
                  + "&miny=" + QString::number( west  * RADIANSTODEGREES )
                  + "&maxx=" + QString::number( north * RADIANSTODEGREES )
                  + "&maxy=" + QString::number( south * RADIANSTODEGREES )
                  + "&size=medium");
    m_downloadManager->addJob( jsonUrl,
                               "panoramio" + QString::number( east ),
                               "panoramio" + QString::number( east ) );
    connect( m_downloadManager, SIGNAL( downloadComplete( QString, QString ) ),
             this,              SLOT( downloadImages( QString , QString ) ) );
}


/**
 * trying to install a event filter on marble widget to filter events for our little image widgets
 */
bool PanoramioPlugin::eventFilter(QObject *object, QEvent *e) {
    MarbleWidget *widget = dynamic_cast<MarbleWidget*> (object);
    if ( !widget ) {
        return RenderPlugin::eventFilter(object, e);
    }
    QMouseEvent *event = static_cast<QMouseEvent*> (e);
    if ( e->type() == QEvent::MouseButtonPress ) {
        qDebug()<<__func__;
    }
}



}
Q_EXPORT_PLUGIN2(PanoramioPlugin, Marble::PanoramioPlugin)

#include "PanoramioPlugin.moc"
