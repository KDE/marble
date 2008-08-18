// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.


//Mine
#include "worldclock.h"

//Qt
#include <QPainter>
#include <QRadialGradient>
#include <QBrush>
#include <QGraphicsSceneHoverEvent>
#include <QList>
#include <QSize>
#include <QRect>

//KDE
#include <KDebug>
#include <KConfigDialog>
#include <KConfigGroup>
#include <KTimeZone>
#include <KSystemTimeZone>

//Plasma
#include <Plasma/Applet>
#include <Plasma/DataEngine>

//Marble
#include "global.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarbleAbstractFloatItem.h"
#include "SunLocator.h"
#include "ViewParams.h"
#include "GeoPainter.h"

//blah compile errors
#include "ViewportParams.h"
#include "AbstractProjection.h"

WorldClock::WorldClock(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_map(0),
    m_sun(0)
{
    setHasConfigurationInterface(true);
    setAcceptHoverEvents(true);
    //The applet needs a 2:1 ratio
    //so that the map fits properly
    resize(QSize(400, 200));
}

void WorldClock::init()
{
    //kDebug() << "Loading available locations...";
    m_locations = KSystemTimeZones::zones();
    QList<QString> zones = m_locations.keys();
    for (int i = 0; i < zones.size(); i++ ) {
        KTimeZone curzone = m_locations.value( zones.at( i ) );
        if ( curzone.latitude() == KTimeZone::UNKNOWN || 
             curzone.longitude() == KTimeZone::UNKNOWN ) {
            m_locations.remove( zones.at(i) );
            //kDebug() << "Removed TZ " << zones.at(i);
        } else {
            //kDebug() << "Kept TZ " << zones.at(i) << " at "
                     //<< curzone.latitude() << "," << curzone.longitude();
        }
    }
    //we'll change the timezone before it's even
    //displayed to the user, so the default zone
    //is not important
    m_locationkey = QString( zones.at( 0 ) );

    //Font sizes will also change before painting
    m_timeFont = QFont( "Helvetica", 12, QFont::Bold);
    m_dateFont = QFont( "Helvetica", 12, QFont::Bold);
    m_locationFont = QFont( "Helvetica", 12, QFont::Bold);

    KConfigGroup cg = config();

    m_timeDisplay = cg.readEntry("timedisplay", 24);

    if (cg.readEntry("showfull", static_cast<int>(Qt::Unchecked)) 
                                               == Qt::Unchecked ) {
        m_showFull = false;
    } else {
        m_showFull = true;
    }

    if (cg.readEntry("showdate", static_cast<int>(Qt::Unchecked)) 
                                               == Qt::Unchecked ) {
        m_showDate = false;
    } else {
        m_showDate = true;
    }

    m_lastRect = geometry().toRect();

    m_map = new MarbleMap(  );
    m_map->setProjection( Equirectangular );

    m_map->setSize(m_lastRect.width(), m_lastRect.height());
    //The radius of the map using this projection 
    //will always be 1/4 of the desired width.
    m_map->setRadius( (m_lastRect.height() / 2 ) );

    //offset so that the date line isn't
    //right on the edge of the map
    //or user choice
    m_map->centerOn( cg.readEntry("rotation", -20), 0 );
    
    //Set how we want the map to look
    m_map->setMapThemeId( "earth/bluemarble/bluemarble.dgml" );
    m_map->setShowCompass( false );
    m_map->setShowClouds( false );
    m_map->setShowScaleBar( false );
    m_map->setShowGrid( false );
    m_map->setShowPlaces( false );
    m_map->setShowCities( false );
    m_map->setShowOtherPlaces( false );
    m_map->setShowRelief( true );
    m_map->setShowIceLayer( true );
    m_map->setShowPlaces( false );
    m_map->setShowOtherPlaces( false );

    foreach( MarbleAbstractFloatItem* item, m_map->model()->floatItems() ) {
        item->setVisible( false );
    }

    //Set up the Sun to draw night/day shadow
    m_sun = m_map->sunLocator();
    m_sun->setShow(true);
    m_sun->setCitylights(true);
    
    if(cg.readEntry("centersun", static_cast<int>(Qt::Unchecked)) 
                                               == Qt::Checked)
         m_sun->setCentered(true);
         
    m_sun->update();
    m_map->updateSun();
    m_map->setNeedsUpdate();

    Plasma::DataEngine *m_timeEngine = dataEngine("time");
    m_timeEngine->connectSource( "Local", this, 6000, 
                                    Plasma::AlignToMinute);

    m_points = QHash<QString, QPoint>();


    //We need to zoom the map every time we change size
    connect(this, SIGNAL(geometryChanged()), this, SLOT(resizeMap()));
}
 
WorldClock::~WorldClock()
{
}
 
void WorldClock::resizeMap()
{
    m_map->setSize(m_lastRect.width(), m_lastRect.height());
    //The radius of the map using this projection 
    //will always be 1/4 of the desired width.
    m_map->setRadius( (m_lastRect.height() / 2 ) );
    m_map->setNeedsUpdate();
    update();
}

void WorldClock::dataUpdated(const QString &source, 
                             const Plasma::DataEngine::Data &data)
{
    //kDebug() << "Time = " << data["Time"].toTime();
    m_localtime = QDateTime( QDate::currentDate(), data["Time"].toTime() );
    m_time = KSystemTimeZones::local().convert(m_locations.value(m_locationkey),
                                               m_localtime );
    //kDebug() << "Adjusted Time = " << m_time;
    m_sun->update();
    m_map->updateSun();
    m_map->setNeedsUpdate();
    update();
}

void WorldClock::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = false;
    Applet::hoverLeaveEvent(event);
    update();
}
void WorldClock::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_isHovered = true;
    m_hover = event->pos();
    setTz( getZone() );
    update();
}
void WorldClock::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    m_hover = event->pos();
    setTz( getZone() );
    update();
}

QString WorldClock::getZone()
{
    //kDebug() << "Finding Timezone";
    qreal lat, lon;
    bool ok = m_map->viewParams()->viewport()->currentProjection()
                                    ->geoCoordinates(                     
                                    m_hover.x(), m_hover.y(),
                                    m_map->viewParams()->viewport(),
                                    lon, lat );
    
    if( ok ) { 
        //kDebug() << "Mouse is at lat " << lat << " lon " << lon;
    } else { 
        //kDebug() << "Mouse lat/lon value lookup FAILED";
        lat = 0;
        lon = 0;
    }
    
    //if( m_locations.isEmpty() ) { kDebug() << "m_locations is EMPTY"; }
    QList<QString> zones = m_locations.keys();
    //kDebug() << "zones: " << zones;
    //default in case lookup fails, which it shouldn't,
    //because if the lookup fails the default SHOULD be 0,0
    QString closest = "America/Toronto";
    
    qreal mindist = 10000;
    
    for (int i = 0; i < zones.size(); i++ ) {
        KTimeZone curzone = m_locations.value( zones.at( i ) );
        qreal tzlon = curzone.longitude();
        qreal tzlat = curzone.latitude();
        qreal londelta, latdelta;
        latdelta = lat - tzlat;
        londelta = lon - tzlon;
        qreal dist = sqrt( (latdelta * latdelta) + (londelta * londelta) );
        //kDebug() << "Distance between mouse and " << zones.at(i)
                 //<< "is " << dist;
        if ( dist < mindist ) {
            mindist = dist;
            closest = zones.at( i );
        }
    }
    //kDebug() << "Found " << m_locations.value( closest ).name();
    return m_locations.value( closest ).name();
}

void WorldClock::setTz( QString newtz )
{
    if ( newtz == m_locationkey ) { return; }
    m_locationkey = newtz;
    m_time = KSystemTimeZones::local().convert(m_locations.value(m_locationkey),
                                               m_localtime );
    recalculateFonts();
}

void WorldClock::recalculatePoints()
{
    int x = m_lastRect.width();
    int y = m_lastRect.height();
    //I originally used "top" as the top, but then decided to put the date
    //over top of that, and didn't want to change everything; so I put "super"
    m_points.insert( "superright", QPoint( ( x*0.666 ), ( y*0.15 ) ) );
    m_points.insert( "superleft", QPoint( ( x*0.333 ), ( y*0.15 ) ) );
    m_points.insert( "topright", QPoint( ( x*0.666 ), ( y*0.25 ) ) );
    m_points.insert( "topleft", QPoint( ( x*0.333 ), ( y*0.25 ) ) );
    m_points.insert( "middleright", QPoint( ( x*0.666 ), ( y*0.58333 ) ) );
    m_points.insert( "middleleft", QPoint( ( x*0.333 ), ( y*0.58333 ) ) );
    m_points.insert( "bottomright", QPoint( ( x*0.666 ), ( y*0.75 ) ) );
    m_points.insert( "bottomleft", QPoint( ( x*0.333 ), ( y*0.75 ) ) );
    return;

}

void WorldClock::recalculateFonts( )
{
    QString timestr;
    if (m_timeDisplay == 12 ) {
        timestr = m_time.toString( "h:mm AP" );
    } else {
        timestr = m_time.toString( "hh:mm" );
    }
    
    QString locstr = m_locationkey;
    if (!m_showFull) {
        locstr.remove( 0, locstr.lastIndexOf( "/" ) + 1 ).replace( "_", " " );
    } 
    QRect timeRect( m_points.value( "topleft" ),
                    m_points.value( "middleright" ) );
    QRect dateRect( m_points.value( "superleft" ), 
                    m_points.value( "topright" ) );
    QRect locationRect( m_points.value( "middleleft" ),
                    m_points.value( "bottomright" ) );
    //kDebug() << "timeRect " << timeRect;
    //kDebug() << "locationRect " << locationRect;
    //we set very small defaults and then increase them
    int lastSize = 3;
    //kDebug() << "Calculating Location Font Size ";
    for ( int curSize = 4; ; curSize++, lastSize++ ) {
        //kDebug() << "trying " << curSize << "pt";
        QFont font( "Helvetica", curSize, QFont::Bold);
        QFontMetrics metrics( font );
        QRect rect = metrics.boundingRect( locstr );
        if ( rect.width()  > locationRect.width() ||
             rect.height() > locationRect.height() ) {
            break;
        }
    }
    //kDebug() << "Using " << lastSize << "pt";
    m_locationFont = QFont( "Helvetica", lastSize, QFont::Bold);
    //kDebug() << "Calculating Time Font Size ";
    lastSize = 3;
    for ( int curSize = 4; ; curSize++, lastSize++ ) {
        //kDebug() << "trying " << curSize << "pt";
        QFont font( "Helvetica", curSize, QFont::Bold);
        QFontMetrics metrics( font );
        QRect rect = metrics.boundingRect( timestr );
        if ( rect.width()  > timeRect.width() ||
             rect.height() > timeRect.height() ) {
            break;
        }
    }
    //kDebug() << "Using " << lastSize << "pt";
    m_timeFont = QFont( "Helvetica", lastSize, QFont::Bold);
    if (m_showDate) {
        //kDebug() << "Calculating Date Font Size ";
        QString datestr = m_time.toString( "ddd d MMM yyyy" );
        lastSize = 3;
        for ( int curSize = 4; ; curSize++, lastSize++ ) {
            //kDebug() << "trying " << curSize << "pt";
            QFont font( "Helvetica", curSize, QFont::Bold);
            QFontMetrics metrics( font );
            QRect rect = metrics.boundingRect( datestr );
            if ( rect.width()  > dateRect.width() ||
                 rect.height() > dateRect.height() ) {
                break;
            }
        }
        //kDebug() << "Using " << lastSize << "pt";
        m_dateFont = QFont( "Helvetica", lastSize, QFont::Bold);
    }
    return;
}

void WorldClock::paintInterface(QPainter *p, 
                                const QStyleOptionGraphicsItem *option,
                                const QRect &contentsRect)
{
    //kDebug() << "contentsRect = " << contentsRect;
    if ( contentsRect != m_lastRect ) { 
        m_lastRect = contentsRect;
        m_map->setSize( m_lastRect.size() );
        m_map->setRadius( m_lastRect.width() / 4 );
        recalculatePoints();
        recalculateFonts();
        resizeMap();
    }
    p->setRenderHint( QPainter::TextAntialiasing , true );
    p->setRenderHint( QPainter::Antialiasing , true );
    QPixmap pixmap( m_lastRect.size() );
    pixmap.fill( Qt::transparent );
    GeoPainter gp( &pixmap, m_map->viewParams()->viewport(), 
                   Marble::Normal, true );
    m_map->paint(gp, m_lastRect);
    p->drawPixmap( 0, 0, pixmap );

    if ( !m_isHovered ) {
        setTz( KSystemTimeZones::local().name() );
    }

    //Show the location on the map
    int tzx = 0;
    int tzy = 0;
    qreal lon = m_locations.value(m_locationkey).longitude();
    qreal lat = m_locations.value(m_locationkey).latitude();
    lon *= DEG2RAD;
    lat *= DEG2RAD;
    //kDebug() << "TZ " << m_locationkey <<  " lon, lat = " << lon << lat;
    bool ok = m_map->viewParams()->viewport()->currentProjection()
              ->screenCoordinates(lon, lat,
                                  m_map->viewParams()->viewport(),
                                  tzx, tzy);
    //kDebug() << "Coordinates are at: " << tzx << tzy;
    if ( ok && m_isHovered ) {
        //kDebug() << "returned x,y = " << tzx << tzy;
        QPoint tz( tzx, tzy );
        int radius = m_lastRect.width() / 40;
        QRadialGradient grad( tz, radius );
        grad.setColorAt( 0,   QColor( 0xFF, 0xFF, 0x00, 0xFF ) );
        grad.setColorAt( 0.33, QColor( 0xFF, 0xFF, 0x00, 0x46 ) );
        grad.setColorAt( 0.66, QColor( 0xFF, 0xFF, 0x00, 0x14 ) );
        grad.setColorAt( 1,   QColor( 0xFF, 0xFF, 0x00, 0x00 ) );
        p->setPen( Qt::NoPen );
        p->setBrush( QBrush( grad ) );
        p->drawEllipse( tz, radius, radius );
    } //else { kDebug() << "Pixel lookup failed!"; }

    p->setPen( QColor( 0xFF, 0xFF, 0xFF ) );

    QString timestr;
    if (m_timeDisplay == 12 ) {
        timestr = m_time.toString( "h:mm AP" );
    } else {
        timestr = m_time.toString( "hh:mm" );
    }
    
    QString locstr = m_locationkey;
    if (m_showFull) {
        locstr.replace( "_", " " );
    } else {
        //remove TZ prefixes
        locstr.remove( 0, locstr.lastIndexOf( "/" ) + 1 ).replace( "_", " " );
    }

    p->setFont( m_timeFont );
    p->drawText( QRect( m_points.value( "topleft" ), 
                        m_points.value( "middleright" ) ),
                 Qt::AlignCenter, timestr );

    p->setFont( m_locationFont );
    p->drawText( QRect( m_points.value( "middleleft" ), 
                        m_points.value( "bottomright" ) ),
                 Qt::AlignCenter, locstr );

    if (m_showDate) {
        QString datestr = m_time.toString( "ddd d MMM yyyy" );
        p->setFont( m_dateFont );
        p->drawText( QRect( m_points.value( "superleft" ), 
                            m_points.value( "topright" ) ),
                     Qt::AlignCenter, datestr );
    }

}

void WorldClock::createConfigurationInterface(KConfigDialog *parent)
{
QWidget *widget = new QWidget();
 ui.setupUi(widget);
 parent->setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel); 

 KConfigGroup cg = config();

     ui.rotationLatLonEdit->setValue(cg.readEntry("rotation", -20));

     if(cg.readEntry("timedisplay", 24)  == 12) {
         ui.timeDisplayComboBox->setCurrentIndex( 1 );
     } else {
         ui.timeDisplayComboBox->setCurrentIndex( 0 );
     }

     if(cg.readEntry("centersun", static_cast<int>(Qt::Unchecked)) 
                                                == Qt::Checked)
          ui.centerSunCheckBox->setChecked(true);

     if(cg.readEntry("showdate", static_cast<int>(Qt::Unchecked)) 
                                               == Qt::Checked)
          ui.showDateCheckBox->setChecked(true);

     if(cg.readEntry("showfull", static_cast<int>(Qt::Unchecked)) 
                                               == Qt::Checked)
          ui.showFullCheckBox->setChecked(true);

     connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
     connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
     parent->addPage(widget, parent->windowTitle(), icon());
}

void WorldClock::configAccepted()
{
    KConfigGroup cg = config();

    if( ui.centerSunCheckBox->checkState() == Qt::Unchecked ) {
        //kDebug() << "CentreSun box is unchecked, m_setCentred(false)";
        m_sun->setCentered(false);
    } else if( ui.centerSunCheckBox->checkState() == Qt::Checked ) {
        //kDebug() << "CentreSun box is checked, m_setCentred(true)";
        m_sun->setCentered(true);
    }

    // What is the centre longitude?
    if( ui.rotationLatLonEdit->value() != cg.readEntry("rotation", -20)  &&
              ui.centerSunCheckBox->checkState() != Qt::Checked ) {
        m_map->centerOn(ui.rotationLatLonEdit->value(), 0);
        update();
    }

    // Do we show the full TZ name?
    if( ui.showFullCheckBox->checkState() !=
            cg.readEntry("showfull", static_cast<int>(Qt::Unchecked)) ) {
        if ( ui.showFullCheckBox->checkState() == Qt::Unchecked ) {
            //kDebug() << "setting m_showFull as false";
            m_showFull = false;
        } else {
            //kDebug() << "setting m_showFull as true";
            m_showFull = true;
        }
        recalculateFonts();
    }

    // Do we show the date?
    if( ui.showDateCheckBox->checkState() !=
            cg.readEntry("showdate", static_cast<int>(Qt::Unchecked)) ) {
        if ( ui.showDateCheckBox->checkState() == Qt::Unchecked ) {
            m_showDate = false;
        } else {
            m_showDate = true;
        }
        recalculateFonts();
    }

    // What type of time display are we using? (12/24hr)
    if(ui.timeDisplayComboBox->currentIndex() != cg.readEntry("showdate", 0)) {
        switch ( ui.timeDisplayComboBox->currentIndex() ) {
            case 1:
                m_timeDisplay = 12;
                cg.writeEntry("timedisplay", 12);
                break;
            //case 0 (and anything else that pops up)
            default:
                m_timeDisplay = 24;
                cg.writeEntry("timedisplay", 24);
                break;
        }
        recalculateFonts();
    }
    
    cg.writeEntry("rotation", ui.rotationLatLonEdit->value());

    cg.writeEntry("centersun", static_cast<int>(ui.centerSunCheckBox->checkState()));
    cg.writeEntry("showfull", static_cast<int>(ui.showFullCheckBox->checkState()));
    cg.writeEntry("showdate", static_cast<int>(ui.showDateCheckBox->checkState()));
}

#include "worldclock.moc"
