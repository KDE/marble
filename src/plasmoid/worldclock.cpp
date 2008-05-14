// Copyright 2008 Henry de Valence
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
#include <QGraphicsSceneHoverEvent>
#include <QList>
#include <QSize>
#include <QRect>



//KDE
#include <KDebug>
#include <KDialog>
#include <KConfigGroup>
#include <KTimeZone>
#include <KSystemTimeZone>


//Plasma
#include <plasma/theme.h>
#include <plasma/dataengine.h>


//Marble
#include "MarbleMap.h"
#include "SunLocator.h"
#include "ViewParams.h"
#include "GeoPainter.h"



WorldClock::WorldClock(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_configDialog(0),
    m_map(0),
    m_sun(0)
{
    setHasConfigurationInterface(true);
    //The applet needs a 2:1 ratio
    //so that the map fits properly
    resize(QSize(400, 200));
}

void WorldClock::init()
{
    loadLocations();
    //we'll change the timezone later
    KTimeZone *m_curtz = new KTimeZone( "Local" );

    KConfigGroup cg = config();

    m_map = new MarbleMap(  );
    m_map->setProjection( Equirectangular );

    m_map->setSize(geometry().size().width(), geometry().size().height());
    //The radius of the map using this projection 
    //will always be 1/4 of the desired width.
    m_map->setRadius( (geometry().size().width() / 4 ) );

    //offset so that the date line isn't
    //right on the edge of the map
    //or user choice
    m_map->centerOn( cg.readEntry("rotation", -20), 0 );
    
    //Set how we want the map to look
    //m_map->setMapTheme( "earth/bluemarble/bluemarble.dgml" );
    // &c.
    m_map->setShowCompass( false );
    m_map->setShowScaleBar( false );
    m_map->setShowGrid( false );
    m_map->setShowPlaces( false );
    m_map->setShowCities( true );
    m_map->setShowOtherPlaces( false );
    m_map->setShowRelief( true );
    m_map->setShowIceLayer( true );
    //Radius*4 = width
    m_map->setRadius( 100 );

    //Set up the Sun to draw night/day shadow
    m_sun = m_map->sunLocator();
    m_sun->setShow(true);
    m_sun->setCitylights(true);
    if(cg.readEntry("centersun", static_cast<int>(Qt::Unchecked)) == Qt::Checked)
         m_sun->setCentered(true);
    m_sun->update();
    m_map->updateSun();

    //We need to zoom the map every time we change size
    connect(this, SIGNAL(geometryChanged()), this, SLOT(resizeMap()));
}
 
WorldClock::~WorldClock()
{
    delete m_configDialog;
}
 
void WorldClock::connectToEngine()
{
    Plasma::DataEngine *m_timeEngine = dataEngine("time");
    m_timeEngine->connectSource( m_curtz->name(), this, 6000, Plasma::AlignToMinute);
}

void WorldClock::resizeMap()
{
    m_map->setSize(geometry().size().width(), geometry().size().height());
    //The radius of the map using this projection 
    //will always be 1/4 of the desired width.
    m_map->setRadius( (geometry().size().width() / 4 ) );
    update();
}
 


void WorldClock::dataUpdated(const QString &name, 
                             const Plasma::DataEngine::Data &data)
{
    m_time = data["Time"].toTime();
    m_city = data["Timezone City"].toString();
    m_sun->update();
    m_map->updateSun();
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
}
void WorldClock::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    m_hover = event->pos();
    setTz( getZone() );
}

KTimeZone WorldClock::getZone()
{
    qDebug() << "Finding Timezone";
    double lat, lon;
    m_map->viewParams()->viewport()->currentProjection()->geoCoordinates(
                                    m_hover.x(), m_hover.y(),
                                    m_map->viewParams()->viewport(),
                                    lon, lat );
    
    qDebug() << "Mouse is at lat " << lat << " lon " << lon;
    
    QList<QString> zones = m_locations.keys();
    QString closest;
    
    double mindist = 10000;
    
    for (int i = 0; i < zones.size(); i++ ) {
        KTimeZone curzone = m_locations.value( zones.at( i ) );
        double tzlon = curzone.longitude();
        double tzlat = curzone.latitude();
        if ( tzlat == KTimeZone::UNKNOWN || 
             tzlon == KTimeZone::UNKNOWN ) {
                continue;
            }
        double dist = sqrt( pow( (lat-tzlat), 2) + pow( (lon-tzlon), 2 ) );
        if ( dist < mindist ) {
            mindist = dist;
            closest = zones.at( i );
        }
    }
    qDebug() << "Found " << m_locations.value( closest ).name();
    return m_locations.value( closest );
}

void WorldClock::setTz( KTimeZone newtz )
{
    qDebug() << "Disconnecting Source: " << m_curtz->name();
    m_timeEngine->disconnectSource( m_curtz->name(), this );
    qDebug() << "Connecting Source: " << newtz.name();
    m_timeEngine->connectSource( newtz.name(), this, 6000, Plasma::AlignToMinute);
    m_curtz = &newtz;
}

void WorldClock::paintInterface(QPainter *p, 
                                const QStyleOptionGraphicsItem *option,
                                const QRect &contentsRect)
{
    kDebug() << contentsRect;
    QRect rect = contentsRect;
    //By creating a pixmap and then painting that
    //we avoid an issue where the map is offset
    //from the border of the plasmoid and it looks ugly
    QPixmap pixmap( rect.size() );
    GeoPainter gp( &pixmap, m_map->viewParams()->viewport(), false );
    m_map->paint(gp, rect);
    p->drawPixmap( 0, 0, pixmap );
    if ( m_isHovered ) {
        QString time = m_time.toString( "h:m" );

        /*
        int width = rect.width() / 4;
        int height = rect.height() / 2;
        QPoint centre = rect.center();

        QRect timerect( 0, 0, width, height );
        timerect.moveCenter( centre );
        timerect.setHeight( height * 0.7 );

        QRect cityrect( 0, 0, width, height );
        cityrect.moveCenter( centre );
        cityrect.setTop( timerect.bottom() );
        */

        p->setFont( QFont( "Helvetica", 12, QFont::Bold) );
        p->drawText( 0,0 , rect.width(), rect.height() / 2.2,
                Qt::AlignHCenter | Qt::AlignBottom, time );

        p->setFont( QFont( "Helvetica", 8, QFont::Bold) );
        p->drawText( 0,0 , rect.width(), rect.height() / 1.8,
                Qt::AlignHCenter | Qt::AlignBottom, m_city );
    }
}

void WorldClock::showConfigurationInterface()
{
    if(m_configDialog == 0) {
         m_configDialog = new KDialog;
	 ui.setupUi(m_configDialog->mainWidget());
	 m_configDialog->setPlainCaption(i18n("Worldclock Applet Configuration"));
         m_configDialog->setButtons(KDialog::Ok | KDialog::Apply | 
	                            KDialog::Cancel); 

         KConfigGroup cg = config();
         ui.rotationLatLonEdit->setValue(cg.readEntry("rotation", -20));
	 if(cg.readEntry("centersun", static_cast<int>(Qt::Unchecked))
                 == Qt::Checked)
              ui.centerSunCheckBox->setChecked(true);

         connect(m_configDialog, SIGNAL(okClicked()), 
	         this, SLOT(configAccepted()));
	 connect(m_configDialog, SIGNAL(applyClicked()), 
	         this, SLOT(configAccepted()));
    }

    m_configDialog->show();
}

void WorldClock::configAccepted()
{
    KConfigGroup cg = config();
    if( ui.centerSunCheckBox->checkState() !=
            cg.readEntry("centersun", static_cast<int>(Qt::Unchecked)) ) {
	switch(ui.centerSunCheckBox->checkState()) {
	    case Qt::Checked :
	        m_sun->setCentered(true);
		break;
	    default :
	        m_sun->setCentered(false);
		break;
        }
	m_sun->update();
        m_map->updateSun();
        update();
    }
    if( ui.rotationLatLonEdit->value() !=
              cg.readEntry("rotation", -20) ) {
	m_map->centerOn(ui.rotationLatLonEdit->value(), 0);
        update();
    }
    cg.writeEntry("centersun", static_cast<int>(ui.centerSunCheckBox->checkState()));
    cg.writeEntry("rotation", ui.rotationLatLonEdit->value());
}

//This function gets the zones but removes those
//which do not have a defined lat/lon.
void WorldClock::loadLocations()
{
    qDebug() << "Loading available locations...";
    QMap<QString, KTimeZone> m_locations = KSystemTimeZones::zones();
    QList<QString> zones = m_locations.keys();
    for (int i = 0; i < zones.size(); i++ ) {
        KTimeZone curzone = m_locations.value( zones.at( i ) );
        if ( curzone.latitude() == KTimeZone::UNKNOWN || 
             curzone.longitude() == KTimeZone::UNKNOWN ) {
            m_locations.remove( zones.at(i) );
            qDebug() << "Removed TZ " << zones.at(i);
        } else {
            qDebug() << "Kept TZ " << zones.at(i) << " at " << curzone.latitude()
                     << "," << curzone.longitude();
        }
    }
}


#include "worldclock.moc"
