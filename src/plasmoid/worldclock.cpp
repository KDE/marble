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
#include <QtGui/QPainter>
#include <QtGui/QRadialGradient>
#include <QtGui/QBrush>
#include <QtGui/QGraphicsSceneHoverEvent>
#include <QtCore/QList>
#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtCore/QTime>
#include <QtCore/QDate>
#include <QtCore/QDateTime>

//KDE
#include <KDebug>
#include <KLocale>
#include <KConfigDialog>
#include <KConfigGroup>
#include <KTimeZone>
#include <KTimeZoneWidget>
#include <KSystemTimeZone>

//Plasma
#include <Plasma/Applet>
#include <Plasma/DataEngine>

//Marble
#include "global.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "AbstractFloatItem.h"
#include "SunLocator.h"
#include "GeoPainter.h"
#include "LatLonEdit.h"
#include "ViewportParams.h"
#include "AbstractProjection.h"

namespace Marble
{

WorldClock::WorldClock(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_map(0),
    m_sun(0)
{
    KGlobal::locale()->insertCatalog("marble");
    KGlobal::locale()->insertCatalog("marble_qt");
    KGlobal::locale()->insertCatalog("timezones4");
    setHasConfigurationInterface(true);
    setAcceptHoverEvents(true);
    //The applet needs a 2:1 ratio
    //so that the map fits properly
    resize(QSize(400, 200));
}

void WorldClock::init()
{
    KConfigGroup cg = config();
    m_map = new MarbleMap();

    if(cg.readEntry("projection", static_cast<int>(Equirectangular)) == Mercator)
        m_map->setProjection(Mercator);
    else
        m_map->setProjection(Equirectangular);

    //Set how we want the map to look
    m_map->centerOn( cg.readEntry("rotation", -20), 0 );
    m_map->setMapThemeId( "earth/bluemarble/bluemarble.dgml" );
    m_map->setShowCompass    ( false );
    m_map->setShowClouds     ( false );
    m_map->setShowScaleBar   ( false );
    m_map->setShowGrid       ( false );
    m_map->setShowPlaces     ( false );
    m_map->setShowCities     ( false );
    m_map->setShowOtherPlaces( false );

    foreach( RenderPlugin* item, m_map->model()->renderPlugins() )
        item->setVisible( false );

    //Set up the Sun to draw night/day shadow
    m_sun = m_map->sunLocator();
    m_sun->setShow(true);
    m_sun->setCitylights(true);
    if(cg.readEntry("centersun", false ))
         m_sun->setCentered(true);

    m_sun->update();
    m_map->updateSun();
    m_map->setNeedsUpdate();

    m_customTz = cg.readEntry("customtz", false );
    m_locationkey = QString(KSystemTimeZones::local().name());
    if(m_customTz) {
        QStringList tzlist = cg.readEntry("tzlist", QStringList());
        m_locations = QMap<QString, KTimeZone>();
        foreach( const QString& tzname, tzlist ) {
            m_locations.insert(tzname, KSystemTimeZones::zone(tzname));
        }
        if(!m_locations.contains(m_locationkey))
            m_locationkey = m_locations.keys().first();
    } else {
        m_locations = KSystemTimeZones::zones();
        QList<QString> zones = m_locations.keys();
        for (int i = 0; i < zones.size(); ++i ) {
            KTimeZone curzone = m_locations.value( zones.at( i ) );
            if ( curzone.latitude() == KTimeZone::UNKNOWN ||
                curzone.longitude() == KTimeZone::UNKNOWN ) {
                m_locations.remove( zones.at(i) );
            }
        }
    }

    //Font sizes will change before painting
    m_timeFont     = QFont( "Helvetica", 12, QFont::Bold);
    m_locationFont = QFont( "Helvetica", 12, QFont::Bold);
    m_points = QHash<QString, QPoint>();
    m_lastRect = QRect(0,0,0,0);
    m_showDate = cg.readEntry("showdate", false);

    setTz( getZone() );

    Plasma::DataEngine *m_timeEngine = dataEngine("time");
    m_timeEngine->connectSource( "Local", this, 6000, Plasma::AlignToMinute);
}

WorldClock::~WorldClock()
{
    delete m_map;
}

void WorldClock::resizeMap(bool changeAspect)
{
    int width = 0;
    int height = 0;
    int radius = 0;
    double ratio = static_cast<double>(m_lastRect.width()) /
                   static_cast<double>(m_lastRect.height());
    if( m_map->projection() == Equirectangular ) {
        kDebug() << "equirectangular with rect" << m_lastRect;
        kDebug() << "w/h ratio:" << ratio;
        if( ratio > 2 ) {
            height = m_lastRect.height();
            width = height*2;
            radius = static_cast<int>(height/2);
        } else {
            width = m_lastRect.width();
            height = static_cast<int>(width/2);
            radius = static_cast<int>(width/4);
        }
    } else if( m_map->projection() == Mercator ) {
        kDebug() << "mercator with rect" << m_lastRect;
        kDebug() << "w/h ratio:" << ratio;
        if( ratio > 1 ) {
            height = m_lastRect.height();
            width = height;
            radius = static_cast<int>(width/4);
        } else {
            width = m_lastRect.width();
            height = width;
            radius = static_cast<int>(width/4);
        }
    }
    kDebug() << "width, height, radius:" << width << height << radius;

    m_map->setSize(width, height);
    m_map->setRadius( radius );
    m_map->setNeedsUpdate();
    update();
    if(changeAspect) {
        QRectF curGeo = geometry();
        setGeometry( curGeo.x(), curGeo.y(), static_cast<double>(width),
                                             static_cast<double>(height) );
    }
}

void WorldClock::dataUpdated(const QString &source,
                             const Plasma::DataEngine::Data &data)
{
    Q_UNUSED(source)
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
    m_hover = event->pos() - m_t;
    Applet::hoverEnterEvent(event);
    setTz( getZone() );
    update();
}
void WorldClock::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    m_hover = event->pos() - m_t;
    Applet::hoverMoveEvent(event);
    setTz( getZone() );
    update();
}

QString WorldClock::getZone()
{
    qreal lat, lon;
    bool ok = m_map->viewport()->currentProjection()->geoCoordinates(
                m_hover.x(), m_hover.y(), m_map->viewport(), lon, lat );

    QString timezone;
    if( !ok ) {
        timezone = KSystemTimeZones::local().name();
        return i18n( timezone.toUtf8().data() );
        }
    QList<QString> zones = m_locations.keys();

    QString closest;
    qreal mindist = 99999999999999999.9;

    for (int i = 0; i < zones.size(); ++i ) {
        KTimeZone cz = m_locations.value( zones.at( i ) );
        qreal dist = sqrt( pow(lat-cz.latitude(), 2) + pow(lon-cz.longitude(), 2) );
        if ( dist < mindist ) {
            mindist = dist;
            closest = zones.at( i );
        }
    }
    timezone = m_locations.value( closest ).name();
    return i18n( timezone.toUtf8().data() );
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
    int x = m_map->width();
    int y = m_map->height();
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
    if(m_showDate)
        timestr = KGlobal::locale()->formatDateTime( m_time );
    else
        timestr = KGlobal::locale()->formatTime( m_time.time() );

    QString locstr = m_locationkey;
    locstr.remove( 0, locstr.lastIndexOf( '/' ) + 1 ).replace( '_', ' ' );
    QRect timeRect( m_points.value( "topleft" ), m_points.value( "middleright" ) );
    QRect locationRect( m_points.value( "middleleft" ), m_points.value( "bottomright" ) );
    
    m_locationFont = calculateFont(locstr, locationRect);
    m_timeFont = calculateFont(timestr, timeRect);
}

QFont WorldClock::calculateFont(const QString &text, const QRect &boundingBox) const
{
    QFont resultFont( "Helvetica", 3, QFont::Bold);
     
    int unscaled = 0; // Avoid infinite loops, bug 189633
    QRect lastBox;
    
    //we set very small defaults and then increase them
    for ( int curSize = resultFont.pointSize()+1; unscaled<100; ++curSize ) {
        resultFont.setPointSize(curSize);
        QFontMetrics metrics( resultFont );
        QRect rect = metrics.boundingRect( text );
        if ( rect.width()  > boundingBox.width() ||
             rect.height() > boundingBox.height() ) {
            break;
        }

        if ( rect.width() > lastBox.width() || 
            rect.height() > lastBox.height() ) {
            unscaled = 0;
        }
        else {
            ++unscaled;
        }

        lastBox = rect;
    }
        
    resultFont.setPointSize(resultFont.pointSize()-1);
    return resultFont;
}

void WorldClock::recalculateTranslation()
{
    m_t = QPoint(static_cast<int>( (m_lastRect.width()/2)  - (m_map->width()/2)   ),
                 static_cast<int>( (m_lastRect.height()/2) - (m_map->height()/2) ));
    m_t += m_lastRect.topLeft();
}

void WorldClock::paintInterface(QPainter *p,
                                const QStyleOptionGraphicsItem *option,
                                const QRect &contentsRect)
{
    Q_UNUSED(option)
    if ( contentsRect != m_lastRect ) {
        m_lastRect = contentsRect;
        resizeMap();
        recalculateTranslation();
            recalculatePoints();
            recalculateFonts();
    }
    p->setRenderHint( QPainter::TextAntialiasing , true );
    p->setRenderHint( QPainter::Antialiasing , true );
    p->setPen( Qt::NoPen );
    //p->setBrush( QBrush( QColor( 0x00, 0x00, 0x00, 0xFF ) ) );
    //p->drawRect( m_lastRect );
    QPixmap pixmap( m_map->width(), m_map->height() );
    pixmap.fill( Qt::transparent );
    GeoPainter gp( &pixmap, m_map->viewport(),
                   Marble::NormalQuality, true );
    QRect mapRect( 0, 0, m_map->width(), m_map->height() );
    m_map->paint(gp, mapRect );
    p->drawPixmap( m_t, pixmap );

    if ( !m_isHovered ) {
        setTz( KSystemTimeZones::local().name() );
    }

    //Show the location on the map
    qreal tzx = 0;
    qreal tzy = 0;
    qreal lon = m_locations.value(m_locationkey).longitude() * DEG2RAD;
    qreal lat = m_locations.value(m_locationkey).latitude() * DEG2RAD;
    bool ok = m_map->viewport()->currentProjection()
              ->screenCoordinates(lon, lat, m_map->viewport(), tzx, tzy);
    if ( ok /*&& m_isHovered*/ ) {
        QPoint tz( tzx, tzy );
        tz += m_t;
        int radius = m_lastRect.width() / 40;
        QRadialGradient grad( tz, radius );
        grad.setColorAt( 0,    QColor( 0xFF, 0xFF, 0x00, 0xFF ) );
        grad.setColorAt( 0.33, QColor( 0xFF, 0xFF, 0x00, 0x46 ) );
        grad.setColorAt( 0.66, QColor( 0xFF, 0xFF, 0x00, 0x14 ) );
        grad.setColorAt( 1,    QColor( 0xFF, 0xFF, 0x00, 0x00 ) );
        p->setBrush( QBrush( grad ) );
        p->drawEllipse( tz, radius, radius );
    }

    p->setPen( QColor( 0xFF, 0xFF, 0xFF ) );

    QString locstr = m_locationkey;
    locstr.remove( 0, locstr.lastIndexOf( '/' ) + 1 ).replace( '_', ' ' );

    QString timestr;
    if(m_showDate)
        timestr = KGlobal::locale()->formatDateTime( m_time );
    else
        timestr = KGlobal::locale()->formatTime( m_time.time() );

    p->setFont( m_timeFont );
    p->drawText( QRect( m_points.value( "topleft" )     + m_t,
                        m_points.value( "middleright" ) + m_t ),
                 Qt::AlignCenter, timestr );

    p->setFont( m_locationFont );
    p->drawText( QRect( m_points.value( "middleleft" )  + m_t,
                        m_points.value( "bottomright" ) + m_t ),
                 Qt::AlignCenter, locstr );
}

void WorldClock::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *widget = new QWidget();
    ui.setupUi(widget);
    parent->setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);

    KConfigGroup cg = config();

    ui.longitudeEdit->setValue(cg.readEntry("rotation", -20));

    if(cg.readEntry("projection", static_cast<int>(Equirectangular)) == Mercator)
        ui.projection->setCurrentIndex(1);
    else //Equirectangular is the default projection
        ui.projection->setCurrentIndex(0);

    if(cg.readEntry("daylight", false ))
        ui.daylightButton->setChecked(true);

    if(cg.readEntry("showdate", false ))
        ui.showdate->setChecked(true);

    if(cg.readEntry("customtz", false ))
        ui.customTz->setChecked(true);

    ui.tzWidget->setSelectionMode( QTreeView::MultiSelection );
    foreach(const QString& tz, cg.readEntry("tzlist")) {
        ui.tzWidget->setSelected(tz,true);
    }

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    parent->addPage(widget, parent->windowTitle(), icon());
}

void WorldClock::configAccepted()
{
    KConfigGroup cg = config();

    if( ui.daylightButton->isChecked() )
        m_sun->setCentered(true);
    else {
        m_map->centerOn(ui.longitudeEdit->value(), 0);
        update();
    }

    m_showDate = ui.showdate->isChecked();
    m_customTz = ui.customTz->isChecked();

    if(m_customTz) {
        QStringList tzlist = ui.tzWidget->selection();
        kDebug() << "\tSetting TZLIST";
        kDebug() << tzlist;
        QMap<QString, KTimeZone> selectedZones;
        selectedZones.insert(KSystemTimeZones::local().name(),
                                KSystemTimeZones::local());
        foreach( const QString& tzname, tzlist ) {
            selectedZones.insert(tzname, KSystemTimeZones::zone(tzname));
        }
        cg.writeEntry("tzlist",tzlist);
        m_locations = selectedZones;
        if(!m_locations.contains(m_locationkey))
            m_locationkey = m_locations.keys().first();
    }

    // What projection? note: +1 because the spherical projection is 0
    if((ui.projection->currentIndex() + 1) != cg.readEntry("projection",
                                          static_cast<int>(Equirectangular))  )
    {
        switch ( ui.projection->currentIndex() ) {
            case 1:
                //kDebug() << "case 1, setting proj to mercator";
                m_map->setProjection(Mercator);
                m_map->setNeedsUpdate(); update();
                resizeMap(true);
                cg.writeEntry("projection", static_cast<int>(Mercator));
                break;
            //case 0 (and anything else that pops up)
            default:
                //kDebug() << "case default, setting proj to Equirectangular";
                m_map->setProjection(Equirectangular);
                m_map->setNeedsUpdate(); update();
                resizeMap(true);
                cg.writeEntry("projection", static_cast<int>(Equirectangular));
                break;
        }
    }

    cg.writeEntry("rotation", ui.longitudeEdit->value());
    cg.writeEntry("centersun", ui.daylightButton->isChecked());
    cg.writeEntry("showdate", ui.showdate->isChecked());
    cg.writeEntry("customtz", ui.customTz->isChecked());

    emit configNeedsSaving();
}

} //ns Marble

#include "worldclock.moc"
