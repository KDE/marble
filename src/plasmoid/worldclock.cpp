//(C) Henry de Valence 2008
/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

//Mine
#include "worldclock.h"


//Qt
#include <QPainter>


//KDE
#include <KDebug>
#include <KDialog>
#include <KConfigGroup>


//Plasma
#include <plasma/theme.h>
#include <plasma/dataengine.h>


//Marble
#include <marble/MarbleMap.h>
#include <marble/SunLocator.h>
#include <marble/ClipPainter.h>



WorldClock::WorldClock(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_configDialog(0),
    m_map(0),
    m_sun(0)
{
    setHasConfigurationInterface(true);
    setDrawStandardBackground(true);
    //The applet needs a 2:1 ratio
    //so that the map fits properly
    resize(QSize(400, 200));
}

void WorldClock::init()
{
    KConfigGroup cg = config();

    m_map = new MarbleMap(  );
    m_map->setProjection( Equirectangular );

    m_map->setSize(contentSize().width(), contentSize().height());
    //The radius of the map using this projection 
    //will always be 1/4 of the desired width.
    m_map->setRadius( (contentSize().width() / 4 ) );

    //offset so that the date line isn't
    //right on the edge of the map
    //or user choice
    m_map->centerOn( cg.readEntry("rotation", -20), 0 );
    
    //Set how we want the map to look
    m_map->setMapTheme( "earth/bluemarble/bluemarble.dgml" );
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
 
//We want to redraw the map every 10 mins
//so that the night/day shade is current.
//10 mins is 1/144th of a rotation.
void WorldClock::connectToEngine()
{
    Plasma::DataEngine* timeEngine = dataEngine("time");
    //update every 5 mins
    timeEngine->connectSource("Local", this, 30000, Plasma::AlignToMinute);
}

void WorldClock::resizeMap()
{
    m_map->setSize(contentSize().width(), contentSize().height());
    //The radius of the map using this projection 
    //will always be 1/4 of the desired width.
    m_map->setRadius( (contentSize().width() / 4 ) );
    update();
}
 


void WorldClock::dataUpdated(const QString &name, 
                             const Plasma::DataEngine::Data &data)
{
    m_sun->update();
    m_map->updateSun();
    update();
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
    ClipPainter cp( &pixmap , false );
    m_map->paint(cp, rect);
    p->drawPixmap( 0, 0, pixmap );
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

#include "worldclock.moc"
