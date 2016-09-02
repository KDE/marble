//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <tackat@kde.org>
// Copyright 2011-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "StarsPlugin.h"

#include "ui_StarsConfigWidget.h"
#include <QRectF>
#include <QSize>
#include <QDateTime>
#include <QRegion>
#include <QContextMenuEvent>
#include <QMenu>
#include <QColorDialog>
#include <qmath.h>

#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "AbstractFloatItem.h"
#include "GeoPainter.h"
#include "Planet.h"
#include "PlanetFactory.h"
#include "SunLocator.h"
#include "ViewportParams.h"

#include "src/lib/astro/solarsystem.h"

namespace Marble
{

StarsPlugin::StarsPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_nameIndex( 0 ),
      m_configDialog( 0 ),
      ui_configWidget( 0 ),
      m_renderStars( true ),
      m_renderConstellationLines( true ),
      m_renderConstellationLabels( true ),
      m_renderDsos( true ),
      m_renderDsoLabels( true ),
      m_renderSun( true ),
      m_renderMoon( true ),
      m_renderEcliptic( true ),
      m_renderCelestialEquator( true ),
      m_renderCelestialPole( true ),
      m_starsLoaded( false ),
      m_starPixmapsCreated( false ),
      m_constellationsLoaded( false ),
      m_dsosLoaded( false ),
      m_zoomSunMoon( true ),
      m_viewSolarSystemLabel( true ),
      m_magnitudeLimit( 100 ),
      m_zoomCoefficient( 4 ),
      m_constellationBrush( Marble::Oxygen::aluminumGray5 ),
      m_constellationLabelBrush( Marble::Oxygen::aluminumGray5 ),
      m_dsoLabelBrush( Marble::Oxygen::aluminumGray5 ),
      m_eclipticBrush( Marble::Oxygen::aluminumGray5 ),
      m_celestialEquatorBrush( Marble::Oxygen::aluminumGray5 ),
      m_celestialPoleBrush( Marble::Oxygen::aluminumGray5 ),
      m_contextMenu(0),
      m_constellationsAction(0),
      m_sunMoonAction(0),
      m_planetsAction(0),
      m_dsoAction(0),
      m_doRender( false )
{
    prepareNames();
}

StarsPlugin::~StarsPlugin()
{
    delete m_contextMenu;
}

QStringList StarsPlugin::backendTypes() const
{
    return QStringList(QStringLiteral("stars"));
}

QString StarsPlugin::renderPolicy() const
{
    return QStringLiteral("SPECIFIED_ALWAYS");
}

QStringList StarsPlugin::renderPosition() const
{
    return QStringList(QStringLiteral("STARS"));
}

RenderPlugin::RenderType StarsPlugin::renderType() const
{
    return RenderPlugin::ThemeRenderType;
}

QString StarsPlugin::name() const
{
    return tr( "Stars" );
}

QString StarsPlugin::guiString() const
{
    return tr( "&Stars" );
}

QString StarsPlugin::nameId() const
{
    return QStringLiteral("stars");
}

QString StarsPlugin::version() const
{
    return QStringLiteral("1.2");
}

QString StarsPlugin::description() const
{
    return tr( "A plugin that shows the Starry Sky and the Sun." );
}

QString StarsPlugin::copyrightYears() const
{
    return QStringLiteral("2008-2012");
}

QVector<PluginAuthor> StarsPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
           << PluginAuthor(QStringLiteral("Torsten Rahn"), QStringLiteral("tackat@kde.org"))
           << PluginAuthor(QStringLiteral("Rene Kuettner"), QStringLiteral("rene@bitkanal.net"))
           << PluginAuthor(QStringLiteral("Timothy Lanzi"), QStringLiteral("trlanzi@gmail.com"));
}

QIcon StarsPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/stars.png"));
}

void StarsPlugin::initialize()
{
}

bool StarsPlugin::isInitialized() const
{
    return true;
}

QDialog *StarsPlugin::configDialog()
{
    if (!m_configDialog) {
        // Initializing configuration dialog
        m_configDialog = new QDialog;
        ui_configWidget = new Ui::StarsConfigWidget;
        ui_configWidget->setupUi( m_configDialog );

        readSettings();

        connect( ui_configWidget->m_buttonBox, SIGNAL(accepted()), SLOT(writeSettings()) );
        connect( ui_configWidget->m_buttonBox, SIGNAL(rejected()), SLOT(readSettings()) );

        connect( ui_configWidget->m_constellationColorButton, SIGNAL(clicked()), this,
                SLOT(constellationGetColor()) );

        connect( ui_configWidget->m_constellationLabelColorButton, SIGNAL(clicked()), this,
                SLOT(constellationLabelGetColor()) );

        connect( ui_configWidget->m_dsoLabelColorButton, SIGNAL(clicked()), this,
                SLOT(dsoLabelGetColor()) );

        connect( ui_configWidget->m_eclipticColorButton, SIGNAL(clicked()), this,
                SLOT(eclipticGetColor()) );

        connect( ui_configWidget->m_celestialEquatorColorButton, SIGNAL(clicked()), this,
                SLOT(celestialEquatorGetColor()) );

        connect( ui_configWidget->m_celestialPoleColorButton, SIGNAL(clicked()), this,
                SLOT(celestialPoleGetColor()) );
    }

    return m_configDialog;
}

QHash<QString, QVariant> StarsPlugin::settings() const
{
    QHash<QString, QVariant> settings = RenderPlugin::settings();

    settings.insert(QStringLiteral("nameIndex"), m_nameIndex);
    settings.insert(QStringLiteral("renderStars"), m_renderStars);
    settings.insert(QStringLiteral("renderConstellationLines"), m_renderConstellationLines);
    settings.insert(QStringLiteral("renderConstellationLabels"), m_renderConstellationLabels);
    settings.insert(QStringLiteral("renderDsos"), m_renderDsos);
    settings.insert(QStringLiteral("renderDsoLabels"), m_renderDsoLabels);
    settings.insert(QStringLiteral("renderSun"), m_renderSun);
    settings.insert(QStringLiteral("renderMoon"), m_renderMoon);

    QStringList planetState;
    foreach (const QString &key, m_renderPlanet.keys())
        planetState += key + QLatin1Char(':') + QString::number((int)m_renderPlanet[key]);
    settings.insert(QStringLiteral("renderPlanet"), planetState.join(QLatin1Char('|')));

    settings.insert(QStringLiteral("renderEcliptic"), m_renderEcliptic);
    settings.insert(QStringLiteral("renderCelestialEquator"), m_renderCelestialEquator);
    settings.insert(QStringLiteral("renderCelestialPole"), m_renderCelestialPole);
    settings.insert(QStringLiteral("zoomSunMoon"), m_zoomSunMoon);
    settings.insert(QStringLiteral("viewSolarSystemLabel"), m_viewSolarSystemLabel);
    settings.insert(QStringLiteral("magnitudeLimit"), m_magnitudeLimit);
    settings.insert(QStringLiteral("constellationBrush"), m_constellationBrush.color().rgb());
    settings.insert(QStringLiteral("constellationLabelBrush"), m_constellationLabelBrush.color().rgb());
    settings.insert(QStringLiteral("dsoLabelBrush"), m_dsoLabelBrush.color().rgb());
    settings.insert(QStringLiteral("eclipticBrush"), m_eclipticBrush.color().rgb());
    settings.insert(QStringLiteral("celestialEaquatorBrush"), m_celestialEquatorBrush.color().rgb());
    settings.insert(QStringLiteral("celestialPoleBrush"), m_celestialPoleBrush.color().rgb());

    return settings;
}

void StarsPlugin::setSettings( const QHash<QString, QVariant> &settings )
{
    RenderPlugin::setSettings( settings );

    m_nameIndex = readSetting<int>(settings, QStringLiteral("nameIndex"), 0);
    m_renderStars = readSetting<bool>(settings, QStringLiteral("renderStars"), true);
    m_renderConstellationLines = readSetting<bool>(settings, QStringLiteral("renderConstellationLines"), true);
    m_renderConstellationLabels = readSetting<bool>(settings, QStringLiteral("renderConstellationLabels"), true);
    m_renderDsos = readSetting<bool>(settings, QStringLiteral("renderDsos"), true);
    m_renderDsoLabels = readSetting<bool>(settings, QStringLiteral("renderDsoLabels"), true);
    m_renderSun = readSetting<bool>(settings, QStringLiteral("renderSun"), true);
    m_renderMoon = readSetting<bool>(settings, QStringLiteral("renderMoon"), true);

    m_renderPlanet.clear();
    const QString renderPlanet = readSetting<QString>(settings, QStringLiteral("renderPlanet"), QString());
    const QStringList renderStates = renderPlanet.split(QLatin1Char('|'));
    foreach(const QString &state, renderStates) {
        const QStringList stateList = state.split(QLatin1Char(':'));
        if (stateList.size() == 2)
            m_renderPlanet[stateList[0]] = (bool)stateList[1].toInt();
    }

    m_renderEcliptic = readSetting<bool>(settings, QStringLiteral("renderEcliptic"), true);
    m_renderCelestialEquator = readSetting<bool>(settings, QStringLiteral("renderCelestialEquator"), true);
    m_renderCelestialPole = readSetting<bool>(settings, QStringLiteral("renderCelestialPole"), true);
    m_zoomSunMoon = readSetting<bool>(settings, QStringLiteral("zoomSunMoon"), true);
    m_viewSolarSystemLabel = readSetting<bool>(settings, QStringLiteral("viewSolarSystemLabel"), true);
    m_magnitudeLimit = readSetting<int>(settings, QStringLiteral("magnitudeLimit"), 100);
    QColor const defaultColor = Marble::Oxygen::aluminumGray5;
    m_constellationBrush = QColor(readSetting<QRgb>(settings, QStringLiteral("constellationBrush"), defaultColor.rgb()));
    m_constellationLabelBrush = QColor(readSetting<QRgb>(settings, QStringLiteral("constellationLabelBrush"), defaultColor.rgb()));
    m_dsoLabelBrush = QColor(readSetting<QRgb>(settings, QStringLiteral("dsoLabelBrush"), defaultColor.rgb()));
    m_eclipticBrush = QColor(readSetting<QRgb>(settings, QStringLiteral("eclipticBrush"), defaultColor.rgb()));
    m_celestialEquatorBrush = QColor(readSetting<QRgb>(settings, QStringLiteral("celestialEquatorBrush"), defaultColor.rgb()));
    m_celestialPoleBrush = QColor(readSetting<QRgb>(settings, QStringLiteral("celestialPoleBrush"), defaultColor.rgb()));
}

QPixmap StarsPlugin::starPixmap(qreal mag, int colorId) const
{
   if ( mag < -1 ) {
       return m_pixN1Stars.at(colorId);
   } else if ( mag < 0 ) {
       return m_pixP0Stars.at(colorId);
   } else if ( mag < 1 ) {
       return m_pixP1Stars.at(colorId);
   } else if ( mag < 2 ) {
       return m_pixP2Stars.at(colorId);
   } else if ( mag < 3 ) {
       return m_pixP3Stars.at(colorId);
   } else if ( mag < 4 ) {
       return m_pixP4Stars.at(colorId);
   } else if ( mag < 5 ) {
       return m_pixP5Stars.at(colorId);
   } else if ( mag < 6 ) {
       return m_pixP6Stars.at(colorId);
   } else {
       return m_pixP7Stars.at(colorId);
   }

   return QPixmap();
}

void StarsPlugin::prepareNames()
{

    QFile names(MarbleDirs::path(QStringLiteral("stars/names.csv")));
    if ( !names.open( QIODevice::ReadOnly ) ) {
        return;
    }

    QTextStream in( &names );
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        const QStringList list = line.split(QLatin1Char(';'));
        if ( list.size() == 3 ) {
            m_nativeHash[ list.at( 0 ) ] = QCoreApplication::translate( "StarNames", list.at( 1 ).toUtf8().constData() );
            m_abbrHash[ list.at( 0 ) ] = list.at( 2 );
        }
    }
    names.close();

}

QString StarsPlugin::assembledConstellation(const QString &name)
{
    switch (m_nameIndex) {
    case 0:
        return name;
    case 1:
        return m_nativeHash[name];
    case 2:
        return m_abbrHash[name];
    default:
        return name;
    }
}

void StarsPlugin::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    ui_configWidget->constellationNamesComboBox->setCurrentIndex(m_nameIndex);

    Qt::CheckState const constellationLineState = m_renderConstellationLines ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewConstellationLinesCheckbox->setCheckState( constellationLineState );

    Qt::CheckState const constellationLabelState = m_renderConstellationLabels ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewConstellationLabelsCheckbox->setCheckState( constellationLabelState );

    Qt::CheckState const dsoState = m_renderDsos ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewDsosCheckbox->setCheckState( dsoState );

    Qt::CheckState const dsoLabelState = m_renderDsoLabels ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewDsoLabelCheckbox->setCheckState( dsoLabelState );

    Qt::CheckState const sunState = m_renderSun ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 0 )->setCheckState( sunState );

    Qt::CheckState const moonState = m_renderMoon ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 1 )->setCheckState( moonState );

    Qt::CheckState const mercuryState = m_renderPlanet["mercury"] ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 2 )->setCheckState(mercuryState);

    Qt::CheckState const venusState = m_renderPlanet["venus"] ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 3 )->setCheckState(venusState);

    Qt::CheckState const marsState = m_renderPlanet["mars"] ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 5 )->setCheckState(marsState);

    Qt::CheckState const jupiterState = m_renderPlanet["jupiter"] ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 6 )->setCheckState(jupiterState);

    Qt::CheckState const saturnState = m_renderPlanet["saturn"] ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 7 )->setCheckState(saturnState);

    Qt::CheckState const uranusState = m_renderPlanet["uranus"] ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 8 )->setCheckState(uranusState);

    Qt::CheckState const neptuneState = m_renderPlanet["neptune"] ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_solarSystemListWidget->item( 9 )->setCheckState(neptuneState);

    Qt::CheckState const eclipticState = m_renderEcliptic ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewEclipticCheckbox->setCheckState( eclipticState );

    Qt::CheckState const celestialEquatorState = m_renderCelestialEquator ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewCelestialEquatorCheckbox->setCheckState( celestialEquatorState );

    Qt::CheckState const celestialPoleState = m_renderCelestialPole ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewCelestialPoleCheckbox->setCheckState( celestialPoleState );

    Qt::CheckState const zoomSunMoonState = m_zoomSunMoon ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_zoomSunMoonCheckbox->setCheckState( zoomSunMoonState );

    Qt::CheckState const viewSolarSystemLabelState = m_viewSolarSystemLabel ? Qt::Checked : Qt::Unchecked;
    ui_configWidget->m_viewSolarSystemLabelCheckbox->setCheckState( viewSolarSystemLabelState );

    int magState = m_magnitudeLimit;
    if ( magState < ui_configWidget->m_magnitudeSlider->minimum() ) {
        magState = ui_configWidget->m_magnitudeSlider->minimum();
    }
    else if ( magState > ui_configWidget->m_magnitudeSlider->maximum() ) {
        magState = ui_configWidget->m_magnitudeSlider->maximum();
    }

    ui_configWidget->m_magnitudeSlider->setValue(magState);

    QPalette constellationPalette;
    constellationPalette.setColor( QPalette::Button, m_constellationBrush.color() );
    ui_configWidget->m_constellationColorButton->setPalette( constellationPalette );

    QPalette constellationLabelPalette;
    constellationLabelPalette.setColor( QPalette::Button, m_constellationLabelBrush.color() );
    ui_configWidget->m_constellationLabelColorButton->setPalette( constellationLabelPalette );

    QPalette dsoLabelPalette;
    dsoLabelPalette.setColor( QPalette::Button, m_dsoLabelBrush.color() );
    ui_configWidget->m_dsoLabelColorButton->setPalette( dsoLabelPalette );

    QPalette eclipticPalette;
    eclipticPalette.setColor( QPalette::Button, m_eclipticBrush.color() );
    ui_configWidget->m_eclipticColorButton->setPalette( eclipticPalette );

    QPalette celestialEquatorPalette;
    celestialEquatorPalette.setColor( QPalette::Button, m_celestialEquatorBrush.color() );
    ui_configWidget->m_celestialEquatorColorButton->setPalette( celestialEquatorPalette );

    QPalette celestialPolePalette;
    celestialPolePalette.setColor( QPalette::Button, m_celestialPoleBrush.color() );
    ui_configWidget->m_celestialPoleColorButton->setPalette( celestialPolePalette );


}

void StarsPlugin::writeSettings()
{
    m_nameIndex = ui_configWidget->constellationNamesComboBox->currentIndex();
    m_renderConstellationLines = ui_configWidget->m_viewConstellationLinesCheckbox->checkState() == Qt::Checked;
    m_renderConstellationLabels = ui_configWidget->m_viewConstellationLabelsCheckbox->checkState() == Qt::Checked;
    m_renderDsos = ui_configWidget->m_viewDsosCheckbox->checkState() == Qt::Checked;
    m_renderDsoLabels = ui_configWidget->m_viewDsoLabelCheckbox->checkState() == Qt::Checked;
    m_renderSun = ui_configWidget->m_solarSystemListWidget->item( 0 )->checkState() == Qt::Checked;
    m_renderMoon = ui_configWidget->m_solarSystemListWidget->item( 1 )->checkState() == Qt::Checked;

    m_renderPlanet["mercury"] = ui_configWidget->m_solarSystemListWidget->item( 2 )->checkState()
            == Qt::Checked;
    m_renderPlanet["venus"] = ui_configWidget->m_solarSystemListWidget->item( 3 )->checkState()
            == Qt::Checked;
    m_renderPlanet["mars"] = ui_configWidget->m_solarSystemListWidget->item( 5 )->checkState()
            == Qt::Checked;
    m_renderPlanet["jupiter"] = ui_configWidget->m_solarSystemListWidget->item( 6 )->checkState()
            == Qt::Checked;
    m_renderPlanet["saturn"] = ui_configWidget->m_solarSystemListWidget->item( 7 )->checkState()
            == Qt::Checked;
    m_renderPlanet["uranus"] = ui_configWidget->m_solarSystemListWidget->item( 8 )->checkState()
            == Qt::Checked;
    m_renderPlanet["neptune"] = ui_configWidget->m_solarSystemListWidget->item( 9 )->checkState()
            == Qt::Checked;

    m_renderEcliptic = ui_configWidget->m_viewEclipticCheckbox->checkState() == Qt::Checked;
    m_renderCelestialEquator = ui_configWidget->m_viewCelestialEquatorCheckbox->checkState() == Qt::Checked;
    m_renderCelestialPole = ui_configWidget->m_viewCelestialPoleCheckbox->checkState() == Qt::Checked;
    m_zoomSunMoon = ui_configWidget->m_zoomSunMoonCheckbox->checkState() == Qt::Checked;
    m_viewSolarSystemLabel = ui_configWidget->m_viewSolarSystemLabelCheckbox->checkState() == Qt::Checked;
    m_magnitudeLimit = ui_configWidget->m_magnitudeSlider->value();
    m_constellationBrush = QBrush( ui_configWidget->m_constellationColorButton->palette().color( QPalette::Button) );
    m_constellationLabelBrush = QBrush( ui_configWidget->m_constellationLabelColorButton->palette().color( QPalette::Button) );
    m_dsoLabelBrush = QBrush( ui_configWidget->m_dsoLabelColorButton->palette().color( QPalette::Button) );
    m_eclipticBrush = QBrush( ui_configWidget->m_eclipticColorButton->palette().color( QPalette::Button) );
    m_celestialEquatorBrush = QBrush( ui_configWidget->m_celestialEquatorColorButton->palette().color( QPalette::Button) );
    m_celestialPoleBrush = QBrush( ui_configWidget->m_celestialPoleColorButton->palette().color( QPalette::Button) );
    emit settingsChanged( nameId() );
}

void StarsPlugin::constellationGetColor()
{
    const QColor c = QColorDialog::getColor( m_constellationBrush.color(), 0, tr("Please choose the color for the constellation lines.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->m_constellationColorButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->m_constellationColorButton->setPalette( palette );
    }
}

void StarsPlugin::constellationLabelGetColor()
{
    const QColor c = QColorDialog::getColor( m_constellationLabelBrush.color(), 0, tr("Please choose the color for the constellation labels.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->m_constellationLabelColorButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->m_constellationLabelColorButton->setPalette( palette );
    }
}

void StarsPlugin::dsoLabelGetColor()
{
    const QColor c = QColorDialog::getColor( m_dsoLabelBrush.color(), 0, tr("Please choose the color for the dso labels.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->m_dsoLabelColorButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->m_dsoLabelColorButton->setPalette( palette );
    }
}

void StarsPlugin::eclipticGetColor()
{
    const QColor c = QColorDialog::getColor( m_eclipticBrush.color(), 0, tr("Please choose the color for the ecliptic.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->m_eclipticColorButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->m_eclipticColorButton->setPalette( palette );
    }
}

void StarsPlugin::celestialEquatorGetColor()
{
    const QColor c = QColorDialog::getColor( m_celestialEquatorBrush.color(), 0, tr("Please choose the color for the celestial equator.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->m_celestialEquatorColorButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->m_celestialEquatorColorButton->setPalette( palette );
    }
}

void StarsPlugin::celestialPoleGetColor()
{
    const QColor c = QColorDialog::getColor( m_celestialPoleBrush.color(), 0, tr("Please choose the color for the celestial equator.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->m_celestialPoleColorButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->m_celestialPoleColorButton->setPalette( palette );
    }
}

void StarsPlugin::loadStars()
{
    //mDebug() << Q_FUNC_INFO;
    // Load star data
    m_stars.clear();

    QFile starFile(MarbleDirs::path(QStringLiteral("stars/stars.dat")));
    starFile.open( QIODevice::ReadOnly );
    QDataStream in( &starFile );

    // Read and check the header
    quint32 magic;
    in >> magic;
    if ( magic != 0x73746172 ) {
        return;
    }

    // Read the version
    qint32 version;
    in >> version;
    if ( version > 004 ) {
        mDebug() << "stars.dat: file too new.";
        return;
    }

    if ( version == 003 ) {
        mDebug() << "stars.dat: file version no longer supported.";
        return;
    }

    int maxid = 0;
    int id = 0;
    int starIndex = 0;
    double ra;
    double de;
    double mag;
    int colorId = 2;

    mDebug() << "Star Catalog Version " << version;

    while ( !in.atEnd() ) {
        if ( version >= 2 ) {
            in >> id;
        }
        if ( id > maxid ) {
            maxid = id;
        }
        in >> ra;
        in >> de;
        in >> mag;

        if ( version >= 4 ) {
            in >> colorId;
        }

        StarPoint star( id, ( qreal )( ra ), ( qreal )( de ), ( qreal )( mag ), colorId );
        // Create entry in stars database
        m_stars << star;
        // Create key,value pair in idHash table to map from star id to
        // index in star database vector
        m_idHash[id] = starIndex;
        // Increment Index for use in hash
        ++starIndex;
    }

    // load the Sun pixmap
    // TODO: adjust pixmap size according to distance
    m_pixmapSun.load(MarbleDirs::path(QStringLiteral("svg/sun.png")));
    m_pixmapMoon.load(MarbleDirs::path(QStringLiteral("svg/moon.png")));

    m_starsLoaded = true;
}

void StarsPlugin::createStarPixmaps()
{
    // Load star pixmaps
    QVector<QPixmap> pixBigStars;
    pixBigStars.clear();
    pixBigStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_0_blue.png"))));
    pixBigStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_0_bluewhite.png"))));
    pixBigStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_0_white.png"))));
    pixBigStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_0_yellow.png"))));
    pixBigStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_0_orange.png"))));
    pixBigStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_0_red.png"))));
    pixBigStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_0_garnetred.png"))));

    QVector<QPixmap> pixSmallStars;
    pixSmallStars.clear();
    pixSmallStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_3_blue.png"))));
    pixSmallStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_3_bluewhite.png"))));
    pixSmallStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_3_white.png"))));
    pixSmallStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_3_yellow.png"))));
    pixSmallStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_3_orange.png"))));
    pixSmallStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_3_red.png"))));
    pixSmallStars.append(QPixmap(MarbleDirs::path(QStringLiteral("bitmaps/stars/star_3_garnetred.png"))));


    // Pre-Scale Star Pixmaps
    m_pixN1Stars.clear();
    for ( int p=0; p < pixBigStars.size(); ++p) {
        int width = 1.0*pixBigStars.at(p).width();
        m_pixN1Stars.append(pixBigStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_pixP0Stars.clear();
    for ( int p=0; p < pixBigStars.size(); ++p) {
        int width = 0.90*pixBigStars.at(p).width();
        m_pixP0Stars.append(pixBigStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_pixP1Stars.clear();
    for ( int p=0; p < pixBigStars.size(); ++p) {
        int width = 0.80*pixBigStars.at(p).width();
        m_pixP1Stars.append(pixBigStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_pixP2Stars.clear();
    for ( int p=0; p < pixBigStars.size(); ++p) {
        int width = 0.70*pixBigStars.at(p).width();
        m_pixP2Stars.append(pixBigStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_pixP3Stars.clear();
    for ( int p=0; p < pixSmallStars.size(); ++p) {
        int width = 14;
        m_pixP3Stars.append(pixSmallStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_pixP4Stars.clear();
    for ( int p=0; p < pixSmallStars.size(); ++p) {
        int width = 10;
        m_pixP4Stars.append(pixSmallStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_pixP5Stars.clear();
    for ( int p=0; p < pixSmallStars.size(); ++p) {
        int width = 6;
        m_pixP5Stars.append(pixSmallStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_pixP6Stars.clear();
    for ( int p=0; p < pixSmallStars.size(); ++p) {
        int width = 4;
        m_pixP6Stars.append(pixSmallStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_pixP7Stars.clear();
    for ( int p=0; p < pixSmallStars.size(); ++p) {
        int width = 1;
        m_pixP7Stars.append(pixSmallStars.at(p).scaledToWidth(width,Qt::SmoothTransformation));
    }

    m_starPixmapsCreated = true;
}

void StarsPlugin::loadConstellations()
{
    // Load star data
    m_constellations.clear();

    QFile constellationFile(MarbleDirs::path(QStringLiteral("stars/constellations.dat")));
    constellationFile.open( QIODevice::ReadOnly );
    QTextStream in( &constellationFile );
    QString line;
    QString indexList;

    while ( !in.atEnd() ) {
        line = in.readLine();

        // Check for null line at end of file
        if ( line.isNull() ) {
            continue;
        }

        // Ignore Comment lines in header and
        // between constellation entries
        if (line.startsWith(QLatin1Char('#'))) {
            continue;
        }

        indexList = in.readLine();

        // Make sure we have a valid label and indexList
        if ( indexList.isNull() ) {
            break;
        }

        Constellation constellation( this, line, indexList );
        m_constellations << constellation;

    }
    m_constellationsLoaded = true;

}

void StarsPlugin::loadDsos()
{
    // Load star data
    m_dsos.clear();

    QFile dsoFile(MarbleDirs::path(QStringLiteral("stars/dso.dat")));
    dsoFile.open( QIODevice::ReadOnly );
    QTextStream in( &dsoFile );
    QString line;

    while ( !in.atEnd() ) {
        line = in.readLine();

        // Check for null line at end of file
        if ( line.isNull() ) {
            continue;
        }

        // Ignore Comment lines in header and
        // between dso entries
        if (line.startsWith(QLatin1Char('#'))) {
            continue;
        }

        QStringList entries = line.split( QLatin1Char( ',' ) );
    
        QString id = entries.at( 0 ); 
        
        double raH = entries.at( 1 ).toDouble();
        double raM = entries.at( 2 ).toDouble();
        double raS = entries.at( 3 ).toDouble();
        double decD = entries.at( 4 ).toDouble();
        double decM = entries.at( 5 ).toDouble();
        double decS = entries.at( 6 ).toDouble();

        double raRad = ( raH+raM/60.0+raS/3600.0 )*15.0*M_PI/180.0;
        double decRad;

        if ( decD >= 0.0 ) {
            decRad = ( decD+decM/60.0+decS/3600.0 )*M_PI/180.0;
        }
        else {
            decRad = ( decD-decM/60.0-decS/3600.0 )*M_PI/180.0;
        }

        DsoPoint dso( id, ( qreal )( raRad ), ( qreal )( decRad ) );
        // Create entry in stars database
        m_dsos << dso;
    }

    m_dsoImage.load(MarbleDirs::path(QStringLiteral("stars/deepsky.png")));
    m_dsosLoaded = true;
}

bool StarsPlugin::render( GeoPainter *painter, ViewportParams *viewport,
                          const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    QString planetId = marbleModel()->planetId();
    const bool doRender = !viewport->mapCoversViewport() &&
                             ( (viewport->projection() == Spherical || viewport->projection() == VerticalPerspective) &&
                             planetId == QLatin1String("earth")); // So far displaying stars is only supported on earth.

    if ( doRender != m_doRender ) {
        if ( doRender ) {
            connect( marbleModel()->clock(), SIGNAL(timeChanged()),
                     this, SLOT(requestRepaint()) );
        } else {
            disconnect( marbleModel()->clock(), SIGNAL(timeChanged()),
                        this, SLOT(requestRepaint()) );
        }

        m_doRender = doRender;
    }

    painter->save();

    SolarSystem sys;
    QDateTime dateTime = marbleModel()->clock()->dateTime();
    sys.setCurrentMJD(
                dateTime.date().year(), dateTime.date().month(), dateTime.date().day(),
                dateTime.time().hour(), dateTime.time().minute(),
                (double)dateTime.time().second());
    QString const pname = planetId.at(0).toUpper() + planetId.right(planetId.size() - 1);
    QByteArray name = pname.toLatin1();
    sys.setCentralBody( name.data() );

    Vec3 skyVector = sys.getPlanetocentric (0.0, 0.0);
    qreal skyRotationAngle = -atan2(skyVector[1], skyVector[0]);

    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();

    const qreal  skyRadius      = 0.6 * sqrt( ( qreal )viewport->width() * viewport->width() + viewport->height() * viewport->height() );

    if ( doRender ) {
        if (!m_starPixmapsCreated) {
            createStarPixmaps();
            m_starPixmapsCreated = true;
        }

        // Delayed initialization:
        // Load the star database only if the sky is actually being painted...
        if ( !m_starsLoaded ) {
            loadStars();
            m_starsLoaded = true;
        }

        if ( !m_constellationsLoaded ) {
            loadConstellations();
            m_constellationsLoaded = true;
        }

        if ( !m_dsosLoaded ) {
            loadDsos();
            m_dsosLoaded = true;
        }

        const qreal  earthRadius    = viewport->radius();

        // List of Pens used to draw the sky
        QPen polesPen( m_celestialPoleBrush, 2, Qt::SolidLine );
        QPen constellationPenSolid( m_constellationBrush, 1, Qt::SolidLine );
        QPen constellationPenDash(  m_constellationBrush, 1, Qt::DashLine );
        QPen constellationLabelPen( m_constellationLabelBrush, 1, Qt::SolidLine );
        QPen eclipticPen( m_eclipticBrush, 1, Qt::DotLine );
        QPen equatorPen( m_celestialEquatorBrush, 1, Qt::DotLine );
        QPen dsoLabelPen (m_dsoLabelBrush, 1, Qt::SolidLine);


        const Quaternion skyAxis = Quaternion::fromEuler( -centerLat , centerLon + skyRotationAngle, 0.0 );
        matrix skyAxisMatrix;
        skyAxis.inverse().toMatrix( skyAxisMatrix );

        if ( m_renderCelestialPole ) {

            polesPen.setWidth( 2 );
            painter->setPen( polesPen );

            Quaternion qpos1;
            qpos1 = Quaternion::fromSpherical( 0, 90 * DEG2RAD );
            qpos1.rotateAroundAxis( skyAxisMatrix );

            if ( qpos1.v[Q_Z] < 0 ) {
                const int x1 = ( int )( viewport->width()  / 2 + skyRadius * qpos1.v[Q_X] );
                const int y1 = ( int )( viewport->height() / 2 - skyRadius * qpos1.v[Q_Y] );
                painter->drawLine( x1, y1, x1+10, y1 );
                painter->drawLine( x1+5, y1-5, x1+5, y1+5 );
                painter->drawText( x1+8, y1+12, "NP" );
            }

            Quaternion qpos2;
            qpos2 = Quaternion::fromSpherical( 0, -90 * DEG2RAD );
            qpos2.rotateAroundAxis( skyAxisMatrix );
            if ( qpos2.v[Q_Z] < 0 ) {
                const int x1 = ( int )( viewport->width()  / 2 + skyRadius * qpos2.v[Q_X] );
                const int y1 = ( int )( viewport->height() / 2 - skyRadius * qpos2.v[Q_Y] );
                painter->drawLine( x1, y1, x1+10, y1 );
                painter->drawLine( x1+5, y1-5, x1+5, y1+5 );
                painter->drawText( x1+8, y1+12, "SP" );
            }
        }

        if( m_renderEcliptic ) {
            const Quaternion eclipticAxis = Quaternion::fromEuler( 0.0, 0.0, -marbleModel()->planet()->epsilon() );
            matrix eclipticAxisMatrix;
            (eclipticAxis * skyAxis).inverse().toMatrix( eclipticAxisMatrix );

            painter->setPen(eclipticPen);

            int previousX = -1;
            int previousY = -1;
            for ( int i = 0; i <= 36; ++i) {
                Quaternion qpos;
                qpos = Quaternion::fromSpherical( i * 10 * DEG2RAD, 0 );
                qpos.rotateAroundAxis( eclipticAxisMatrix );

                int x = ( int )( viewport->width()  / 2 + skyRadius * qpos.v[Q_X] );
                int y = ( int )( viewport->height() / 2 - skyRadius * qpos.v[Q_Y] );

                if ( qpos.v[Q_Z] < 0 && previousX >= 0 ) painter->drawLine(previousX, previousY, x, y);

                previousX = x;
                previousY = y;
            }
        }

        if( m_renderCelestialEquator ) {
            painter->setPen(equatorPen);

            int previousX = -1;
            int previousY = -1;
            for ( int i = 0; i <= 36; ++i) {
                Quaternion qpos;
                qpos = Quaternion::fromSpherical( i * 10 * DEG2RAD, 0 );
                qpos.rotateAroundAxis( skyAxisMatrix );

                int x = ( int )( viewport->width()  / 2 + skyRadius * qpos.v[Q_X] );
                int y = ( int )( viewport->height() / 2 - skyRadius * qpos.v[Q_Y] );

                if ( qpos.v[Q_Z] < 0 && previousX > 0 ) painter->drawLine(previousX, previousY, x, y);

                previousX = x;
                previousY = y;
            }
        }

        if ( m_renderDsos ) {
            painter->setPen(dsoLabelPen);
            // Render Deep Space Objects
            for ( int d = 0; d < m_dsos.size(); ++d ) {
                Quaternion qpos = m_dsos.at( d ).quaternion();
                qpos.rotateAroundAxis( skyAxisMatrix );

                if ( qpos.v[Q_Z] > 0 ) {
                    continue;
                }

                qreal earthCenteredX = qpos.v[Q_X] * skyRadius;
                qreal earthCenteredY = qpos.v[Q_Y] * skyRadius;

                // Don't draw high placemarks (e.g. satellites) that aren't visible.
                if ( qpos.v[Q_Z] < 0
                        && ( ( earthCenteredX * earthCenteredX
                               + earthCenteredY * earthCenteredY )
                             < earthRadius * earthRadius ) ) {
                    continue;
                }

                // Let (x, y) be the position on the screen of the placemark..
                const int x = ( int )( viewport->width()  / 2 + skyRadius * qpos.v[Q_X] );
                const int y = ( int )( viewport->height() / 2 - skyRadius * qpos.v[Q_Y] );

                // Skip placemarks that are outside the screen area
                if ( x < 0 || x >= viewport->width() ||
                     y < 0 || y >= viewport->height() ) {
                    continue;
                }

                // Hard Code DSO Size for now
                qreal size = 20;

                // Center Image on x,y location
                painter->drawImage( QRectF( x-size/2, y-size/2, size, size ),m_dsoImage );
                if (m_renderDsoLabels) {
                    painter->drawText( x+8, y+12, m_dsos.at( d ).id() );
                }
            }
        }

        if ( m_renderConstellationLines ||  m_renderConstellationLabels )
        {
            // Render Constellations
            for ( int c = 0; c < m_constellations.size(); ++c ) {
                int xMean = 0;
                int yMean = 0;
                int endptCount = 0;
                painter->setPen( constellationPenSolid );

                for ( int s = 0; s < ( m_constellations.at( c ).size() - 1 ); ++s ) {
                    int starId1 = m_constellations.at( c ).at( s );
                    int starId2 = m_constellations.at( c ).at( s + 1 );

                    if ( starId1 == -1 || starId2 == -1 ) {
                        // starId == -1 means we don't draw this segment
                        continue;
                    } else if ( starId1 == -2 || starId2 == -2 ) {
                        painter->setPen( constellationPenDash );
                    } else if ( starId1 == -3 || starId2 == -3 ) {
                        painter->setPen( constellationPenSolid );
                    }

                    int idx1 = m_idHash.value( starId1,-1 );
                    int idx2 = m_idHash.value( starId2,-1 );

                   
                    if ( idx1 < 0 ) {
                        mDebug() << "unknown star, "
                                 << starId1 <<  ", in constellation "
                                 << m_constellations.at( c ).name();
                        continue;
                    }

                    if ( idx2 < 0 ) {
                        mDebug() << "unknown star, "
                                 << starId1 <<  ", in constellation "
                                 << m_constellations.at( c ).name();
                        continue;
                    }
                    // Fetch quaternion from star s in constellation c
                    Quaternion q1 = m_stars.at( idx1 ).quaternion();
                    // Fetch quaternion from star s+1 in constellation c
                    Quaternion q2 = m_stars.at( idx2 ).quaternion();

                    q1.rotateAroundAxis( skyAxisMatrix );
                    q2.rotateAroundAxis( skyAxisMatrix );

                    if ( q1.v[Q_Z] > 0 || q2.v[Q_Z] > 0 ) {
                        continue;
                    }


                    // Let (x, y) be the position on the screen of the placemark..
                    int x1 = ( int )( viewport->width()  / 2 + skyRadius * q1.v[Q_X] );
                    int y1 = ( int )( viewport->height() / 2 - skyRadius * q1.v[Q_Y] );
                    int x2 = ( int )( viewport->width()  / 2 + skyRadius * q2.v[Q_X] );
                    int y2 = ( int )( viewport->height() / 2 - skyRadius * q2.v[Q_Y] );


                    xMean = xMean + x1 + x2;
                    yMean = yMean + y1 + y2;
                    endptCount = endptCount + 2;

                    if ( m_renderConstellationLines ) {
                        painter->drawLine( x1, y1, x2, y2 );
                    }

                }

                // Skip constellation labels that are outside the screen area
                if ( endptCount > 0 ) {
                    xMean = xMean / endptCount;
                    yMean = yMean / endptCount;
                }

                if ( endptCount < 1 || xMean < 0 || xMean >= viewport->width()
                        || yMean < 0 || yMean >= viewport->height() )
                    continue;

                painter->setPen( constellationLabelPen );
                if ( m_renderConstellationLabels ) {
                    painter->drawText( xMean, yMean, m_constellations.at( c ).name() );
                }

            }
        }

        // Render Stars

        for ( int s = 0; s < m_stars.size(); ++s  ) {
            Quaternion  qpos = m_stars.at(s).quaternion();

            qpos.rotateAroundAxis( skyAxisMatrix );

            if ( qpos.v[Q_Z] > 0 ) {
                continue;
            }

            qreal  earthCenteredX = qpos.v[Q_X] * skyRadius;
            qreal  earthCenteredY = qpos.v[Q_Y] * skyRadius;

            // Don't draw high placemarks (e.g. satellites) that aren't visible.
            if ( qpos.v[Q_Z] < 0
                    && ( ( earthCenteredX * earthCenteredX
                           + earthCenteredY * earthCenteredY )
                         < earthRadius * earthRadius ) ) {
                continue;
            }

            // Let (x, y) be the position on the screen of the placemark..
            const int x = ( int )( viewport->width()  / 2 + skyRadius * qpos.v[Q_X] );
            const int y = ( int )( viewport->height() / 2 - skyRadius * qpos.v[Q_Y] );

            // Skip placemarks that are outside the screen area
            if ( x < 0 || x >= viewport->width()
                    || y < 0 || y >= viewport->height() )
                continue;

            // Show star if it is brighter than magnitude threshold
            if ( m_stars.at(s).magnitude() < m_magnitudeLimit ) {

                // colorId is used to select which pixmap in vector to display
                int colorId = m_stars.at(s).colorId();
                QPixmap s_pixmap = starPixmap(m_stars.at(s).magnitude(), colorId);
                int sizeX = s_pixmap.width();
                int sizeY = s_pixmap.height();
                painter->drawPixmap( x-sizeX/2, y-sizeY/2 ,s_pixmap );
            }
        }

        if ( m_renderSun ) {
            // sun
            double ra = 0.0;
            double decl = 0.0;
            sys.getSun( ra, decl );
            ra = 15.0 * sys.DmsDegF( ra );
            decl = sys.DmsDegF( decl );

            Quaternion qpos = Quaternion::fromSpherical( ra * DEG2RAD, decl * DEG2RAD );
            qpos.rotateAroundAxis( skyAxisMatrix );

            if ( qpos.v[Q_Z] <= 0 ) {
                QPixmap glow(MarbleDirs::path(QStringLiteral("svg/glow.png")));
                qreal deltaX  = glow.width()  / 2.;
                qreal deltaY  = glow.height() / 2.;
                int x = (int)(viewport->width()  / 2 + skyRadius * qpos.v[Q_X]);
                int y = (int)(viewport->height() / 2 - skyRadius * qpos.v[Q_Y]);

                bool glowDrawn = false;
                if (!(x < -glow.width() || x >= viewport->width() ||
                      y < -glow.height() || y >= viewport->height())) {
                    painter->drawPixmap( x - deltaX, y - deltaY, glow );
                    glowDrawn = true;
                }

                if (glowDrawn) {
                    double diameter = 0.0, mag = 0.0;
                    sys.getPhysSun(diameter, mag);
                    const int coefficient = m_zoomSunMoon ? m_zoomCoefficient : 1;
                    const qreal size = skyRadius * qSin(diameter) * coefficient;
                    const qreal factor = size/m_pixmapSun.width();
                    QPixmap sun = m_pixmapSun.transformed(QTransform().scale(factor, factor),
                                                          Qt::SmoothTransformation);
                    deltaX  = sun.width()  / 2.;
                    deltaY  = sun.height() / 2.;
                    x = (int)(viewport->width()  / 2 + skyRadius * qpos.v[Q_X]);
                    y = (int)(viewport->height() / 2 - skyRadius * qpos.v[Q_Y]);

                    painter->drawPixmap( x - deltaX, y - deltaY, sun );
                }

                // It's labels' time!
                if (m_viewSolarSystemLabel)
                    painter->drawText(x+deltaX*1.5, y+deltaY*1.5, tr("Sun"));
            }
        }

        if ( m_renderMoon && marbleModel()->planetId() == QLatin1String("earth")) {
            // moon
            double ra=0.0;
            double decl=0.0;
            sys.getMoon(ra, decl);
            ra = 15.0 * sys.DmsDegF(ra);
            decl = sys.DmsDegF(decl);

            Quaternion qpos = Quaternion::fromSpherical( ra * DEG2RAD,
                                                         decl * DEG2RAD );
            qpos.rotateAroundAxis( skyAxisMatrix );

            if ( qpos.v[Q_Z] <= 0 ) {
                // If zoom Sun and Moon is enabled size is multiplied by zoomCoefficient.
                const int coefficient = m_zoomSunMoon ? m_zoomCoefficient : 1;

                QPixmap moon = m_pixmapMoon.copy();

                const qreal size = skyRadius * qSin(sys.getDiamMoon()) * coefficient;
                qreal deltaX  = size  / 2.;
                qreal deltaY  = size / 2.;
                const int x = (int)(viewport->width()  / 2 + skyRadius * qpos.v[Q_X]);
                const int y = (int)(viewport->height() / 2 - skyRadius * qpos.v[Q_Y]);


                if (!(x < -size || x >= viewport->width() ||
                      y < -size || y >= viewport->height())) {
                    // Moon phases
                    double phase = 0.0, ildisk = 0.0, amag = 0.0;
                    sys.getLunarPhase(phase, ildisk, amag);

                    QPainterPath path;

                    QRectF fullMoonRect = moon.rect();
                    if (ildisk < 0.05) {
                        // small enough, so it's not visible
                        path.addEllipse(fullMoonRect);
                    } else if (ildisk < 0.95) { // makes sense to do smth
                        QRectF halfEllipseRect;
                        qreal ellipseWidth = 2 * qAbs(ildisk-0.5) * moon.width();
                        halfEllipseRect.setX((fullMoonRect.width() - ellipseWidth) * 0.5);
                        halfEllipseRect.setWidth(ellipseWidth);
                        halfEllipseRect.setHeight(moon.height());

                        if (phase < 0.5) {
                            if (ildisk < 0.5) {
                                path.moveTo(fullMoonRect.width()/2, moon.height());
                                path.arcTo(fullMoonRect, -90, -180);
                                path.arcTo(halfEllipseRect, 90, -180);
                            } else {
                                path.moveTo(fullMoonRect.width()/2, 0);
                                path.arcTo(fullMoonRect, 90, 180);
                                path.arcTo(halfEllipseRect, -90, -180);
                            }
                        } else {
                            if (ildisk < 0.5) {
                                path.moveTo(fullMoonRect.width()/2, moon.height());
                                path.arcTo(fullMoonRect, -90, 180);
                                path.arcTo(halfEllipseRect, 90, 180);
                            } else {
                                path.moveTo(fullMoonRect.width()/2, 0);
                                path.arcTo(fullMoonRect, 90, -180);
                                path.arcTo(halfEllipseRect, -90, 180);
                            }
                        }

                        path.closeSubpath();
                    }

                    QPainter overlay;
                    overlay.begin(&moon);
                    overlay.setPen(Qt::NoPen);
                    overlay.setBrush(QBrush(QColor(0, 0, 0, 180)));
                    overlay.setRenderHint(QPainter::Antialiasing, true);
                    overlay.drawPath(path);
                    overlay.end();

                    qreal angle = marbleModel()->planet()->epsilon() * qCos(ra * DEG2RAD) * RAD2DEG;
                    if (viewport->polarity() < 0) angle += 180;

                    QTransform form;
                    const qreal factor = size / moon.size().width();
                    moon = moon.transformed(form.rotate(angle).scale(factor, factor),
                                                            Qt::SmoothTransformation);

                    painter->drawPixmap( x - deltaX, y - deltaY, moon );

                    // It's labels' time!
                    if (m_viewSolarSystemLabel)
                        painter->drawText(x+deltaX, y+deltaY, PlanetFactory::localizedName("moon"));
                }
            }
        }

        foreach(const QString &planet, m_renderPlanet.keys()) {
            if (m_renderPlanet[planet])
                renderPlanet(planet, painter, sys, viewport, skyRadius, skyAxisMatrix);
        }
    }

    painter->restore();

    return true;
}

void StarsPlugin::renderPlanet(const QString &planetId,
                               GeoPainter *painter,
                               SolarSystem &sys,
                               ViewportParams *viewport,
                               qreal skyRadius,
                               matrix &skyAxisMatrix) const
{
    double ra(.0), decl(.0), diam(.0), mag(.0), phase(.0);
    int color=0;

    // venus, mars, jupiter, uranus, neptune, saturn
    if (planetId == QLatin1String("venus")) {
        sys.getVenus(ra, decl);
        sys.getPhysVenus(diam, mag, phase);
        color = 2;
    } else if (planetId == QLatin1String("mars")) {
        sys.getMars(ra, decl);
        sys.getPhysMars(diam, mag, phase);
        color = 5;
    } else if (planetId == QLatin1String("jupiter")) {
        sys.getJupiter(ra, decl);
        sys.getPhysJupiter(diam, mag, phase);
        color = 2;
    } else if (planetId == QLatin1String("mercury")) {
        sys.getMercury(ra, decl);
        sys.getPhysMercury(diam, mag, phase);
        color = 3;
    } else if (planetId == QLatin1String("saturn")) {
        sys.getSaturn(ra, decl);
        sys.getPhysSaturn(diam, mag, phase);
        color = 3;
    } else if (planetId == QLatin1String("uranus")) {
        sys.getUranus(ra, decl);
        sys.getPhysUranus(diam, mag, phase);
        color = 0;
    } else if (planetId == QLatin1String("neptune")) {
        sys.getNeptune(ra, decl);
        sys.getPhysNeptune(diam, mag, phase);
        color = 0;
    } else {
        return;
    }

    ra = 15.0 * sys.DmsDegF(ra);
    decl = sys.DmsDegF(decl);

    Quaternion qpos = Quaternion::fromSpherical( ra * DEG2RAD,
                                                 decl * DEG2RAD );
    qpos.rotateAroundAxis( skyAxisMatrix );

    if ( qpos.v[Q_Z] <= 0 ) {
        QPixmap planetPixmap = starPixmap(mag, color);

        qreal deltaX  = planetPixmap.width()  / 2.;
        qreal deltaY  = planetPixmap.height() / 2.;
        const int x = (int)(viewport->width()  / 2 + skyRadius * qpos.v[Q_X]);
        const int y = (int)(viewport->height() / 2 - skyRadius * qpos.v[Q_Y]);

        if (!(x < 0 || x >= viewport->width() ||
             y < 0 || y >= viewport->height())) {
             painter->drawPixmap( x - deltaX, y - deltaY, planetPixmap );
        }

        // It's labels' time!
        if (m_viewSolarSystemLabel)
            painter->drawText(x+deltaX, y+deltaY, PlanetFactory::localizedName(planetId));
    }
}

void StarsPlugin::requestRepaint()
{
    emit repaintNeeded( QRegion() );
}

void StarsPlugin::toggleSunMoon(bool on)
{
    m_renderSun = on;
    m_renderMoon = on;
    if (on) {
        m_viewSolarSystemLabel = true;
    }

    const Qt::CheckState state = on ? Qt::Checked : Qt::Unchecked;
    if ( m_configDialog ) {
        ui_configWidget->m_solarSystemListWidget->item( 0 )->setCheckState( state );
        ui_configWidget->m_solarSystemListWidget->item( 1 )->setCheckState( state );
        ui_configWidget->m_viewSolarSystemLabelCheckbox->setChecked(m_viewSolarSystemLabel);
    }
    emit settingsChanged( nameId() );
    requestRepaint();
}

void StarsPlugin::toggleDsos(bool on)
{
    m_renderDsos = on;
    // only enable lables if set to true
    if (on) {
        m_renderDsoLabels = true;
    }

    const Qt::CheckState state = on ? Qt::Checked : Qt::Unchecked;
    if ( m_configDialog ) {
        ui_configWidget->m_viewDsosCheckbox->setChecked(state);
        ui_configWidget->m_viewDsoLabelCheckbox->setChecked(state);
    }
    emit settingsChanged( nameId() );
    requestRepaint();
}

void StarsPlugin::toggleConstellations(bool on)
{
    m_renderConstellationLines = on;
    m_renderConstellationLabels = on;

    const Qt::CheckState state = on ? Qt::Checked : Qt::Unchecked;
    if ( m_configDialog ) {
        ui_configWidget->m_viewConstellationLinesCheckbox->setChecked( state );
        ui_configWidget->m_viewConstellationLabelsCheckbox->setChecked( state );
    }
    emit settingsChanged( nameId() );
    requestRepaint();
}

void StarsPlugin::togglePlanets(bool on)
{
    m_renderPlanet["venus"] = on;
    m_renderPlanet["mars"]  = on;
    m_renderPlanet["jupiter"] = on;
    m_renderPlanet["mercury"] = on;
    m_renderPlanet["saturn"] = on;
    m_renderPlanet["uranus"] = on;
    m_renderPlanet["neptune"] = on;

    const Qt::CheckState state = on ? Qt::Checked : Qt::Unchecked;
    if ( m_configDialog ) {
        // Mercury, Venus, Mars, Jupiter, Saturn, Uranus, Neptune
        ui_configWidget->m_solarSystemListWidget->item(2)->setCheckState(state);
        ui_configWidget->m_solarSystemListWidget->item(3)->setCheckState(state);
        ui_configWidget->m_solarSystemListWidget->item(5)->setCheckState(state);
        ui_configWidget->m_solarSystemListWidget->item(6)->setCheckState(state);
        ui_configWidget->m_solarSystemListWidget->item(7)->setCheckState(state);
        ui_configWidget->m_solarSystemListWidget->item(8)->setCheckState(state);
        ui_configWidget->m_solarSystemListWidget->item(9)->setCheckState(state);
    }

    emit settingsChanged( nameId() );
    requestRepaint();
}

void StarsPlugin::executeConfigDialog()
{
    QDialog *dialog = configDialog();
    Q_ASSERT( dialog );
    dialog->exec();
}

bool StarsPlugin::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    if( e->type() == QEvent::ContextMenu )
    {
        MarbleWidget *widget = dynamic_cast<MarbleWidget *>( object );
        QContextMenuEvent *menuEvent = dynamic_cast<QContextMenuEvent *> ( e );
        if( widget && menuEvent )
        {
            qreal mouseLon, mouseLat;
            const bool aboveMap = widget->geoCoordinates( menuEvent->x(), menuEvent->y(),
                                                     mouseLon, mouseLat, GeoDataCoordinates::Radian );
            if ( aboveMap ) {
                return false;
            }

            foreach ( AbstractFloatItem *floatItem, widget->floatItems() ) {
                if ( floatItem->enabled() && floatItem->visible()
                     && floatItem->contains( menuEvent->pos() ) )
                {
                    return false;
                }
            }

            if (!m_contextMenu) {
                m_contextMenu = new QMenu;
                m_constellationsAction = m_contextMenu->addAction(tr("Show &Constellations"),
                                                                  this, SLOT(toggleConstellations(bool)));
                m_constellationsAction->setCheckable(true);

                m_sunMoonAction = m_contextMenu->addAction(tr("Show &Sun and Moon"),
                                                           this, SLOT(toggleSunMoon(bool)));
                m_sunMoonAction->setCheckable(true);

                m_planetsAction = m_contextMenu->addAction(tr("Show &Planets"),
                                                           this, SLOT(togglePlanets(bool)));
                m_planetsAction->setCheckable(true);

                m_dsoAction = m_contextMenu->addAction(tr("Show &Deep Sky Objects"),
                                                       this, SLOT(toggleDsos(bool)) );
                m_dsoAction->setCheckable(true);

                m_contextMenu->addSeparator();
                m_contextMenu->addAction(tr("&Configure..."),
                                         this, SLOT(executeConfigDialog()));
            }

            // update action states
            m_constellationsAction->setChecked(m_renderConstellationLines || m_renderConstellationLabels);
            m_sunMoonAction->setChecked(m_renderSun || m_renderMoon);
            m_dsoAction->setChecked(m_renderDsos);
            const bool isAnyPlanetRendered =
                m_renderPlanet["venus"] ||   m_renderPlanet["mars"] ||
                m_renderPlanet["jupiter"] || m_renderPlanet["mercury"] ||
                m_renderPlanet["saturn"] ||  m_renderPlanet["uranus"] ||
                m_renderPlanet["neptune"];
            m_planetsAction->setChecked(isAnyPlanetRendered);

            m_contextMenu->exec(widget->mapToGlobal(menuEvent->pos()));
            return true;
        }
        return false;
    } else {
        return RenderPlugin::eventFilter( object, e );
    }
}

}

#include "moc_StarsPlugin.cpp"
