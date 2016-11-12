//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>
//

#include "GraticulePlugin.h"
#include "ui_GraticuleConfigWidget.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataLineString.h"
#include "Planet.h"
#include "MarbleModel.h"
#include "PluginAboutDialog.h"

#include "ViewportParams.h"
#include "GeoDataLatLonAltBox.h"

// Qt
#include <QPushButton>
#include <QRect>
#include <QColor>
#include <QBrush>
#include <QColorDialog>
#include <QDebug>



namespace Marble
{

GraticulePlugin::GraticulePlugin()
    : RenderPlugin( 0 ),
      m_showPrimaryLabels( true ),
      m_showSecondaryLabels( true ),
      m_isInitialized( false ),
      ui_configWidget( 0 ),
      m_configDialog( 0 )
{
}

GraticulePlugin::GraticulePlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_equatorCirclePen( Qt::yellow ),
      m_tropicsCirclePen( Qt::yellow ),
      m_gridCirclePen( Qt::white ),
      m_showPrimaryLabels( true ),
      m_showSecondaryLabels( true ),
      m_isInitialized( false ),
      ui_configWidget( 0 ),
      m_configDialog( 0 )
{
}

QStringList GraticulePlugin::backendTypes() const
{
    return QStringList(QStringLiteral("graticule"));
}

QString GraticulePlugin::renderPolicy() const
{
    return QStringLiteral("ALWAYS");
}

QStringList GraticulePlugin::renderPosition() const
{
    return QStringList(QStringLiteral("GRATICULE"));
}

QString GraticulePlugin::name() const
{
    return tr( "Coordinate Grid" );
}

QString GraticulePlugin::guiString() const
{
    return tr( "Coordinate &Grid" );
}

QString GraticulePlugin::nameId() const
{
    return QStringLiteral("coordinate-grid");
}

QString GraticulePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString GraticulePlugin::description() const
{
    return tr( "A plugin that shows a coordinate grid." );
}

QString GraticulePlugin::copyrightYears() const
{
    return QStringLiteral("2009");
}

QVector<PluginAuthor> GraticulePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Torsten Rahn"), QStringLiteral("tackat@kde.org"));
}

QIcon GraticulePlugin::icon () const
{
    return QIcon(QStringLiteral(":/icons/coordinate.png"));
}

void GraticulePlugin::initialize ()
{
    // Initialize range maps that map the zoom to the number of coordinate grid lines.
    
    initLineMaps( GeoDataCoordinates::defaultNotation() );                

    m_isInitialized = true;
}

bool GraticulePlugin::isInitialized () const
{
    return m_isInitialized;
}

QDialog *GraticulePlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::GraticuleConfigWidget;
        ui_configWidget->setupUi( m_configDialog );

        connect( ui_configWidget->gridPushButton, SIGNAL(clicked()), this,
                SLOT(gridGetColor()) );
        connect( ui_configWidget->tropicsPushButton, SIGNAL(clicked()), this,
                SLOT(tropicsGetColor()) );
        connect( ui_configWidget->equatorPushButton, SIGNAL(clicked()), this,
                SLOT(equatorGetColor()) );


        connect( ui_configWidget->m_buttonBox, SIGNAL(accepted()), this, 
                SLOT(writeSettings()) );
        connect( ui_configWidget->m_buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()),
                 SLOT(restoreDefaultSettings()) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()),
                 this,        SLOT(writeSettings()) );
    }

    readSettings();

    return m_configDialog;
}


QHash<QString,QVariant> GraticulePlugin::settings() const
{
    QHash<QString, QVariant> settings = RenderPlugin::settings();

    settings.insert(QStringLiteral("gridColor"), m_gridCirclePen.color().name());
    settings.insert(QStringLiteral("tropicsColor"), m_tropicsCirclePen.color().name());
    settings.insert(QStringLiteral("equatorColor"), m_equatorCirclePen.color().name());
    settings.insert(QStringLiteral("primaryLabels"), m_showPrimaryLabels);
    settings.insert(QStringLiteral("secondaryLabels"), m_showSecondaryLabels);

    return settings;
}

void GraticulePlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    RenderPlugin::setSettings( settings );

    const QColor gridColor = settings.value(QStringLiteral("gridColor"), QColor(Qt::white)).value<QColor>();
    const QColor tropicsColor = settings.value(QStringLiteral("tropicsColor"), QColor(Qt::yellow)).value<QColor>();
    const QColor equatorColor = settings.value(QStringLiteral("equatorColor"), QColor(Qt::yellow)).value<QColor>();
    bool primaryLabels = settings.value(QStringLiteral("primaryLabels"), true).toBool();
    bool secondaryLabels = settings.value(QStringLiteral("secondaryLabels"), true).toBool();

    m_gridCirclePen.setColor( gridColor );
    m_tropicsCirclePen.setColor( tropicsColor );
    m_equatorCirclePen.setColor( equatorColor );

    m_showPrimaryLabels = primaryLabels;
    m_showSecondaryLabels = secondaryLabels;

    readSettings();
}


void GraticulePlugin::readSettings()
{
    if ( !m_configDialog )
        return;

    QPalette gridPalette;
    gridPalette.setColor( QPalette::Button, m_gridCirclePen.color() );
    ui_configWidget->gridPushButton->setPalette( gridPalette );

    QPalette tropicsPalette;
    tropicsPalette.setColor( QPalette::Button, m_tropicsCirclePen.color() );
    ui_configWidget->tropicsPushButton->setPalette( tropicsPalette );


    QPalette equatorPalette;
    equatorPalette.setColor( QPalette::Button, m_equatorCirclePen.color() );
    ui_configWidget->equatorPushButton->setPalette( equatorPalette );
    ui_configWidget->primaryCheckBox->setChecked( m_showPrimaryLabels );
    ui_configWidget->secondaryCheckBox->setChecked( m_showSecondaryLabels );
}

void GraticulePlugin::gridGetColor()
{
    const QColor c = QColorDialog::getColor( m_gridCirclePen.color(), 0, tr("Please choose the color for the coordinate grid.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->gridPushButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->gridPushButton->setPalette( palette );
    }
}

void GraticulePlugin::tropicsGetColor()
{
    const QColor c = QColorDialog::getColor( m_tropicsCirclePen.color(), 0, tr("Please choose the color for the tropic circles.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->tropicsPushButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->tropicsPushButton->setPalette( palette );
    }
}

void GraticulePlugin::equatorGetColor()
{
    const QColor c = QColorDialog::getColor( m_equatorCirclePen.color(), 0, tr("Please choose the color for the equator.") );

    if ( c.isValid() ) {
        QPalette palette = ui_configWidget->equatorPushButton->palette();
        palette.setColor( QPalette::Button, c );
        ui_configWidget->equatorPushButton->setPalette( palette );
    }
}

void GraticulePlugin::writeSettings()
{
    m_equatorCirclePen.setColor( ui_configWidget->equatorPushButton->palette().color( QPalette::Button ) );
    m_tropicsCirclePen.setColor( ui_configWidget->tropicsPushButton->palette().color( QPalette::Button ) );
    m_gridCirclePen.setColor( ui_configWidget->gridPushButton->palette().color( QPalette::Button) );
    m_showPrimaryLabels = ui_configWidget->primaryCheckBox->isChecked();
    m_showSecondaryLabels = ui_configWidget->secondaryCheckBox->isChecked();

    emit settingsChanged( nameId() );
}

bool GraticulePlugin::render( GeoPainter *painter, ViewportParams *viewport,
				const QString& renderPos,
				GeoSceneLayer * layer )
{
    Q_UNUSED( layer )
    Q_UNUSED( renderPos )

    if ( m_currentNotation != GeoDataCoordinates::defaultNotation() ) {
        initLineMaps( GeoDataCoordinates::defaultNotation() );
    }

    // Setting the label font for the coordinate lines.
#ifdef Q_OS_MACX
    int defaultFontSize = 10;
#else
    int defaultFontSize = 8;
#endif

    QFont gridFont("Sans Serif");
    gridFont.setPointSize( defaultFontSize );    
    gridFont.setBold( true );

    painter->save();

    painter->setFont( gridFont );

    renderGrid( painter, viewport, m_equatorCirclePen, m_tropicsCirclePen, m_gridCirclePen );

    painter->restore();

    return true;
}

qreal GraticulePlugin::zValue() const
{
    return 1.0;
}

void GraticulePlugin::renderGrid( GeoPainter *painter, ViewportParams *viewport,
                                  const QPen& equatorCirclePen,
                                  const QPen& tropicsCirclePen,
                                  const QPen& gridCirclePen )
{
    GeoDataLatLonAltBox viewLatLonAltBox = viewport->viewLatLonAltBox();

    painter->setPen( equatorCirclePen );

    LabelPositionFlags mainPosition(NoLabel);
    if ( m_showPrimaryLabels ) {
        mainPosition = LineCenter;
    }
    // Render the equator
    renderLatitudeLine( painter, 0.0, viewLatLonAltBox, tr( "Equator" ), mainPosition );

    // Render the Prime Meridian and Antimeridian
    GeoDataCoordinates::Notation notation = GeoDataCoordinates::defaultNotation();
    if (marbleModel()->planet()->id() != QLatin1String("sky") && notation != GeoDataCoordinates::Astro) {
        renderLongitudeLine( painter, 0.0, viewLatLonAltBox, 0.0, 0.0, tr( "Prime Meridian" ), mainPosition );
        renderLongitudeLine( painter, 180.0, viewLatLonAltBox, 0.0, 0.0, tr( "Antimeridian" ), mainPosition );
    }

    painter->setPen( gridCirclePen );
    // painter->setPen( QPen( QBrush( Qt::white ), 0.75 ) );

    // Render UTM grid zones
    if ( m_currentNotation == GeoDataCoordinates::UTM ) {
        renderLatitudeLine( painter, 84.0, viewLatLonAltBox );

        renderLongitudeLines( painter, viewLatLonAltBox,
                    6.0, 0.0,
                    18.0, 154.0, LineEnd | IgnoreXMargin );
        renderLongitudeLines( painter, viewLatLonAltBox,
                    6.0, 0.0,
                    34.0, 10.0, LineStart | IgnoreXMargin );

        // Paint longitudes with exceptions
        renderLongitudeLines( painter, viewLatLonAltBox,
                    6.0, 0.0,
                    6.0, 162.0, LineEnd | IgnoreXMargin );
        renderLongitudeLines( painter, viewLatLonAltBox,
                    6.0, 0.0,
                    26.0, 146.0, LineEnd | IgnoreXMargin  );

        renderLatitudeLines( painter, viewLatLonAltBox, 8.0, 0.0 /*,
                             LineStart | IgnoreYMargin */ );

        return;
    }

    // Render the normal grid

    // calculate the angular distance between coordinate lines of the normal grid
    qreal normalDegreeStep = 360.0 / m_normalLineMap.lowerBound(viewport->radius()).value();

    LabelPositionFlags labelXPosition(NoLabel), labelYPosition(NoLabel);
    if ( m_showSecondaryLabels ) {
        labelXPosition = LineStart | IgnoreXMargin;
        labelYPosition = LineStart | IgnoreYMargin;
    }
    qreal boldDegreeStep = 360.0 / m_boldLineMap.lowerBound(viewport->radius()).value();
    renderLongitudeLines( painter, viewLatLonAltBox,
                          normalDegreeStep, boldDegreeStep,
                          normalDegreeStep, normalDegreeStep,
                          labelXPosition );
    renderLatitudeLines(  painter, viewLatLonAltBox, normalDegreeStep, boldDegreeStep,
                          labelYPosition );

    // Render some non-cut off longitude lines ..
    renderLongitudeLine( painter, +90.0, viewLatLonAltBox );
    renderLongitudeLine( painter, -90.0, viewLatLonAltBox );

    // Render the bold grid

    if (    painter->mapQuality() == HighQuality
         || painter->mapQuality() == PrintQuality ) {

        QPen boldPen = gridCirclePen;
        boldPen.setWidthF( 2.0 );
        painter->setPen( boldPen );
    }

    // calculate the angular distance between coordinate lines of the bold grid

    renderLongitudeLines( painter, viewLatLonAltBox,
                        boldDegreeStep, 0.0,
                        normalDegreeStep, normalDegreeStep,
                        NoLabel
                        );

    renderLatitudeLines(  painter, viewLatLonAltBox, boldDegreeStep, 0.0,
                        NoLabel );

    QPen tropicsPen = tropicsCirclePen;
    if (   painter->mapQuality() != OutlineQuality
        && painter->mapQuality() != LowQuality ) {
        tropicsPen.setStyle( Qt::DotLine );
    }
    painter->setPen( tropicsPen );

    // Determine the planet's axial tilt
    qreal axialTilt = RAD2DEG * marbleModel()->planet()->epsilon();

    if ( axialTilt > 0 ) {
        // Render the tropics
        renderLatitudeLine( painter, +axialTilt, viewLatLonAltBox, tr( "Tropic of Cancer" ), mainPosition  );
        renderLatitudeLine( painter, -axialTilt, viewLatLonAltBox, tr( "Tropic of Capricorn" ), mainPosition );

        // Render the arctics
        renderLatitudeLine( painter, +90.0 - axialTilt, viewLatLonAltBox, tr( "Arctic Circle" ), mainPosition );
        renderLatitudeLine( painter, -90.0 + axialTilt, viewLatLonAltBox, tr( "Antarctic Circle" ), mainPosition );
    }    
}

void GraticulePlugin::renderLatitudeLine( GeoPainter *painter, qreal latitude,
                                          const GeoDataLatLonAltBox& viewLatLonAltBox,
                                          const QString& lineLabel,
                                          LabelPositionFlags labelPositionFlags )
{
    qreal fromSouthLat = viewLatLonAltBox.south( GeoDataCoordinates::Degree );
    qreal toNorthLat   = viewLatLonAltBox.north( GeoDataCoordinates::Degree );

    // Coordinate line is not displayed inside the viewport
    if ( latitude < fromSouthLat || toNorthLat < latitude ) {
        // mDebug() << "Lat: Out of View";
        return;
    }

    GeoDataLineString line( Tessellate | RespectLatitudeCircle ) ;

    qreal fromWestLon = viewLatLonAltBox.west( GeoDataCoordinates::Degree );
    qreal toEastLon   = viewLatLonAltBox.east( GeoDataCoordinates::Degree );

    if ( fromWestLon < toEastLon ) {
        qreal step = ( toEastLon - fromWestLon ) * 0.25;

        for ( int i = 0; i < 5; ++i ) {
            line << GeoDataCoordinates( fromWestLon + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
        }
    }
    else {
        qreal step = ( +180.0 - toEastLon ) * 0.25;

        for ( int i = 0; i < 5; ++i ) {
            line << GeoDataCoordinates( toEastLon + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
        }

        step = ( +180 + fromWestLon ) * 0.25;

        for ( int i = 0; i < 5; ++i ) {
            line << GeoDataCoordinates( -180 + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
        }
    }

    painter->drawPolyline( line, lineLabel, labelPositionFlags, painter->pen().color() );
}

void GraticulePlugin::renderLongitudeLine( GeoPainter *painter, qreal longitude,
                                           const GeoDataLatLonAltBox& viewLatLonAltBox, 
                                           qreal northPolarGap, qreal southPolarGap,
                                           const QString& lineLabel,
                                           LabelPositionFlags labelPositionFlags )
{
    const qreal fromWestLon = viewLatLonAltBox.west();
    const qreal toEastLon   = viewLatLonAltBox.east();

    // Coordinate line is not displayed inside the viewport
    if ( ( !viewLatLonAltBox.crossesDateLine() 
           && ( longitude * DEG2RAD < fromWestLon || toEastLon < longitude * DEG2RAD ) ) ||
         (  viewLatLonAltBox.crossesDateLine() &&
            longitude * DEG2RAD < toEastLon && fromWestLon < longitude * DEG2RAD &&
            fromWestLon != -M_PI && toEastLon != +M_PI )
       ) {
        // mDebug() << "Lon: Out of View:" << viewLatLonAltBox.toString() << " Crossing: "<< viewLatLonAltBox.crossesDateLine() << "Longitude: " << longitude;
        return;
    }

    qreal fromSouthLat = viewLatLonAltBox.south( GeoDataCoordinates::Degree );
    qreal toNorthLat   = viewLatLonAltBox.north( GeoDataCoordinates::Degree );
    
    qreal southLat = ( fromSouthLat < -90.0 + southPolarGap ) ? -90.0 + southPolarGap : fromSouthLat;
    qreal northLat = ( toNorthLat   > +90.0 - northPolarGap ) ? +90.0 - northPolarGap : toNorthLat;

    GeoDataCoordinates n1( longitude, southLat, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n3( longitude, northLat, 0.0, GeoDataCoordinates::Degree );

    GeoDataLineString line( Tessellate );

    if ( northLat > 0 && southLat < 0 )
    {
        GeoDataCoordinates n2( longitude, 0.0, 0.0, GeoDataCoordinates::Degree );
        line << n1 << n2 << n3;
    }
    else {
        line << n1 << n3;
    }

    painter->drawPolyline( line, lineLabel, labelPositionFlags, painter->pen().color() );
}

void GraticulePlugin::renderLatitudeLines( GeoPainter *painter,
                                           const GeoDataLatLonAltBox& viewLatLonAltBox,
                                           qreal step, qreal skipStep,
                                           LabelPositionFlags labelPositionFlags
                                         )
{
    if ( step <= 0 ) {
        return;
    }

    // Latitude
    qreal southLat = viewLatLonAltBox.south( GeoDataCoordinates::Degree );
    qreal northLat = viewLatLonAltBox.north( GeoDataCoordinates::Degree );

    qreal southLineLat = step * static_cast<int>( southLat / step ); 
    qreal northLineLat = step * ( static_cast<int>( northLat / step ) + 1 );
    
    if ( m_currentNotation == GeoDataCoordinates::UTM ) {
        if ( northLineLat > 84.0 ) {
            northLineLat = 76.0;
        }

        if ( southLineLat < -80.0 ) {
            southLineLat = -80.0;
        }
    }

    qreal itStep = southLineLat;

    GeoDataCoordinates::Notation notation = GeoDataCoordinates::defaultNotation();

    while ( itStep < northLineLat ) {
        // Create a matching label
        QString label = GeoDataCoordinates::latToString( itStep, notation,
                                 GeoDataCoordinates::Degree, -1, 'g' );

        // No additional labels for the equator
        if ( labelPositionFlags.testFlag( LineCenter ) && itStep == 0.0 ) {
            label.clear();
        }

        // Paint all latitude coordinate lines except for the equator
        if ( itStep != 0.0 && fmod(itStep, skipStep) != 0 ) {
            renderLatitudeLine( painter, itStep, viewLatLonAltBox, label, labelPositionFlags );
        }

        itStep += step;
    }
}


void GraticulePlugin::renderUtmExceptions( GeoPainter *painter,
                                            const GeoDataLatLonAltBox& viewLatLonAltBox,
                                            qreal itStep, qreal northPolarGap, qreal southPolarGap,
                                            const QString & label,
                                            LabelPositionFlags labelPositionFlags )
{
    // This code renders the so called "exceptions" in the UTM coordinate grid
    // See: http://en.wikipedia.org/wiki/Universal_Transverse_Mercator_coordinate_system#Exceptions
    if ( northPolarGap == 6.0 && southPolarGap == 162.0) {
        if (label == QLatin1String("33")) {
            renderLongitudeLine( painter, itStep-3.0, viewLatLonAltBox, northPolarGap,
            southPolarGap, label, labelPositionFlags );
        } else if (label == QLatin1String("35")) {
            renderLongitudeLine( painter, itStep-3.0, viewLatLonAltBox, northPolarGap,
            southPolarGap, label, labelPositionFlags );
        } else if (label == QLatin1String("37")) {
            renderLongitudeLine( painter, itStep-3.0, viewLatLonAltBox, northPolarGap,
            southPolarGap, label, labelPositionFlags );
        } else if (label == QLatin1String("32") || label == QLatin1String("34") || label == QLatin1String("36")) {
            // paint nothing
        } else {
            renderLongitudeLine( painter, itStep, viewLatLonAltBox, northPolarGap,
            southPolarGap, label, labelPositionFlags );
        }
    }
    else if ( northPolarGap == 26.0 && southPolarGap == 146.0 ) {
        if (label == QLatin1String("32")) {
            renderLongitudeLine( painter, itStep-3.0, viewLatLonAltBox, northPolarGap,
            southPolarGap, label, labelPositionFlags );
        } else {
            renderLongitudeLine( painter, itStep, viewLatLonAltBox, northPolarGap,
            southPolarGap, label, labelPositionFlags );
        }
    }
    else {
        renderLongitudeLine( painter, itStep, viewLatLonAltBox, northPolarGap,
        southPolarGap, label, labelPositionFlags );
    }
}

void GraticulePlugin::renderLongitudeLines( GeoPainter *painter, 
                                            const GeoDataLatLonAltBox& viewLatLonAltBox, 
                                            qreal step, qreal skipStep,
                                            qreal northPolarGap, qreal southPolarGap,
                                            LabelPositionFlags labelPositionFlags )
{
    if ( step <= 0 ) {
        return;
    }

    const bool isSky = (marbleModel()->planet()->id() == QLatin1String("sky"));
    const GeoDataCoordinates::Notation notation = isSky ? GeoDataCoordinates::Astro : GeoDataCoordinates::defaultNotation();

    // Set precision to 0 in UTM in order to show only zone number.
    int precision = (notation == GeoDataCoordinates::UTM) ? 0 : -1;

    // Longitude
    qreal westLon = viewLatLonAltBox.west( GeoDataCoordinates::Degree );
    qreal eastLon = viewLatLonAltBox.east( GeoDataCoordinates::Degree );

    qreal westLineLon = step * static_cast<int>( westLon / step );
    qreal eastLineLon = step * ( static_cast<int>( eastLon / step ) + 1 );

    if ( !viewLatLonAltBox.crossesDateLine() ||
         ( westLon == -180.0 && eastLon == +180.0 ) ) {
        qreal itStep = westLineLon;

        while ( itStep < eastLineLon ) {
            // Create a matching label
            QString label = GeoDataCoordinates::lonToString( itStep,
                                  notation, GeoDataCoordinates::Degree,
                                  precision, 'g' );

            // No additional labels for the prime meridian and the antimeridian

            if ( labelPositionFlags.testFlag( LineCenter ) && ( itStep == 0.0 || itStep == 180.0 || itStep == -180.0 ) )
            {
                label.clear();
            }

            // Paint all longitude coordinate lines (except for the meridians in non-UTM mode)
            if (notation == GeoDataCoordinates::UTM ) {
                renderUtmExceptions( painter, viewLatLonAltBox, itStep, northPolarGap,
                southPolarGap, label, labelPositionFlags );
            } else if ( itStep != 0.0 && itStep != 180.0 && itStep != -180.0 ) {
                if (fmod(itStep, skipStep) != 0 || skipStep == 0.0) {
                    renderLongitudeLine( painter, itStep, viewLatLonAltBox, northPolarGap,
                    southPolarGap, label, labelPositionFlags );
                }
            }
            itStep += step;
        }
    }
    else {
        qreal itStep = eastLineLon;

        while ( itStep < 180.0 ) {
            // Create a matching label
            QString label = GeoDataCoordinates::lonToString( itStep,
                                  notation, GeoDataCoordinates::Degree,
                                  precision, 'g' );

            // No additional labels for the prime meridian and the antimeridian

            if ( labelPositionFlags.testFlag( LineCenter ) && ( itStep == 0.0 || itStep == 180.0 || itStep == -180.0 ) )
            {
                label.clear();
            }

            // Paint all longitude coordinate lines (except for the meridians in non-UTM mode)
            if (notation == GeoDataCoordinates::UTM ) {
                renderUtmExceptions( painter, viewLatLonAltBox, itStep, northPolarGap,
                southPolarGap, label, labelPositionFlags );
            } else if ( itStep != 0.0 && itStep != 180.0 && itStep != -180.0 ) {
                if (fmod((itStep), skipStep) != 0 || skipStep == 0.0) {
                    renderLongitudeLine( painter, itStep, viewLatLonAltBox, northPolarGap,
                    southPolarGap, label, labelPositionFlags );
                }
            }
            itStep += step;
        }

        itStep = -180.0;

        while ( itStep < westLineLon ) {
            // Create a matching label
            QString label = GeoDataCoordinates::lonToString( itStep,
                                  notation, GeoDataCoordinates::Degree,
                                  precision, 'g' );

            // No additional labels for the prime meridian and the antimeridian
            if ( labelPositionFlags.testFlag( LineCenter ) && ( itStep == 0.0 || itStep == 180.0 || itStep == -180.0 ) )
            {
                label.clear();
            }

            // Paint all longitude coordinate lines (except for the meridians in non-UTM mode)
            if (notation == GeoDataCoordinates::UTM ) {
                renderUtmExceptions( painter, viewLatLonAltBox, itStep, northPolarGap,
                southPolarGap, label, labelPositionFlags );
            } else if ( itStep != 0.0 && itStep != 180.0 && itStep != -180.0 ) {
                if (fmod((itStep+180), skipStep) != 0 || skipStep == 0.0) {
                    renderLongitudeLine( painter, itStep, viewLatLonAltBox, northPolarGap,
                    southPolarGap, label, labelPositionFlags );
                }
            }
            itStep += step;
        }
    }
}

void GraticulePlugin::initLineMaps( GeoDataCoordinates::Notation notation)
{
    /* Define Upper Bound keys and associated values:
       The key number is the globe radius in pixel.
       The value number is the amount of grid lines for the full range.

       Example: up to a 100 pixel radius the globe is covered
       with 4 longitude lines (4 half-circles).
     */

    if (marbleModel()->planet()->id() == QLatin1String("sky") ||
        notation == GeoDataCoordinates::Astro) {
        m_normalLineMap[100]     = 4;          // 6h
        m_normalLineMap[1000]    = 12;          // 2h
        m_normalLineMap[2000]   = 24;         // 1h
        m_normalLineMap[4000]   = 48;         // 30 min
        m_normalLineMap[8000]   = 96;         // 15 min
        m_normalLineMap[16000]  = 288;        // 5 min
        m_normalLineMap[100000]  = 24 * 60;     // 1 min
        m_normalLineMap[200000]  = 24 * 60 * 2; // 30 sec
        m_normalLineMap[400000]  = 24 * 60 * 4; // 15 sec
        m_normalLineMap[1200000] = 24 * 60 * 12; // 5 sec
        m_normalLineMap[6000000] = 24 * 60 * 60; // 1 sec
        m_normalLineMap[12000000] = 24 * 60 * 60 * 2; // 0.5 sec
        m_normalLineMap[24000000] = 24 * 60 * 60 * 4; // 0.25 sec

        m_boldLineMap[1000]     = 0;        // 0h
        m_boldLineMap[2000]    = 4;         //  6h
        m_boldLineMap[16000]    = 24;       //  30 deg
        return;
    }

    m_normalLineMap[100]     = 4;          // 90 deg
    m_normalLineMap[1000]    = 12;          // 30 deg
    m_normalLineMap[4000]   = 36;         // 10 deg
    m_normalLineMap[16000]   = 72;         // 5 deg
    m_normalLineMap[64000]  = 360;         //  1 deg
    m_normalLineMap[128000] = 720;        //  0.5 deg

    m_boldLineMap[1000]     = 0;         //  0 deg
    m_boldLineMap[4000]    = 12;         //  30 deg
    m_boldLineMap[16000]   = 36;         //  10 deg

    switch ( notation )
    {
        case GeoDataCoordinates::Decimal :
            
            m_normalLineMap[512000]  = 360 * 10;       //  0.1 deg
            m_normalLineMap[2048000] = 360 * 20;       //  0.05 deg
            m_normalLineMap[8192000] = 360 * 100;      //  0.01 deg
            m_normalLineMap[16384000] = 360 * 200;      //  0.005 deg
            m_normalLineMap[32768000] = 360 * 1000;    //  0.001 deg
            m_normalLineMap[131072000] = 360 * 2000;    //  0.0005 deg
            m_normalLineMap[524288000] = 360 * 10000;  //  0.00001 deg

            m_boldLineMap[512000]     = 360;          // 0.1 deg
            m_boldLineMap[2048000]    = 720;          // 0.05 deg
            m_boldLineMap[8192000]   = 360 * 10;     // 0.01 deg
            m_boldLineMap[1638400]   = 360 * 20;     // 0.005 deg
            m_boldLineMap[32768000]  = 360 * 100;    // 0.001 deg
            m_boldLineMap[131072000]  = 360 * 200;    // 0.0005 deg
            m_boldLineMap[524288000] = 360 * 1000;   // 0.00001 deg

        break;
        default:
        case GeoDataCoordinates::DMS :            
            m_normalLineMap[512000]  = 360 * 6;         //  10'
            m_normalLineMap[1024000] = 360 * 12;        //  5'
            m_normalLineMap[4096000] = 360 * 60;        //  1'
            m_normalLineMap[8192000] = 360 * 60 * 2;    //  30"
            m_normalLineMap[16384000] = 360 * 60 * 6;   //  10"
            m_normalLineMap[65535000] = 360 * 60 * 12;  //  5"
            m_normalLineMap[524288000] = 360 * 60 * 60; //  1"

            m_boldLineMap[512000]     = 360;          // 10'
            m_boldLineMap[1024000]    = 720;          // 5'
            m_boldLineMap[4096000]   = 360 * 6;      // 1'
            m_boldLineMap[8192000]   = 360 * 12;     // 30"
            m_boldLineMap[16384000]  = 360 * 60;     // 10"
            m_boldLineMap[65535000]  = 360 * 60 * 2; // 5"
            m_boldLineMap[524288000] = 360 * 60 * 6; // 1"

        break;
    }
    m_normalLineMap[999999999] = m_normalLineMap.value(262144000);     //  last
    m_boldLineMap[999999999]   = m_boldLineMap.value(262144000);     //  last

    m_currentNotation = notation;
}

}

#include "moc_GraticulePlugin.cpp"
