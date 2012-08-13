//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#include "OverviewMap.h"

#include <QtCore/QRect>
#include <QtCore/QStringList>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QColorDialog>

#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "ui_OverviewMapConfigWidget.h"

#include "GeoDataPoint.h"
#include "ViewportParams.h"
#include "MarbleWidget.h"
#include "Planet.h"

namespace Marble
{

OverviewMap::OverviewMap()
    : AbstractFloatItem( 0 ),
      ui_configWidget( 0 ),
      m_configDialog( 0 )
{
}

OverviewMap::OverviewMap( const MarbleModel *marbleModel )
    : AbstractFloatItem( marbleModel, QPointF( 10.5, 10.5 ), QSizeF( 166.0, 86.0 ) ),
      m_target( QString() ),
      m_planetID( Planet::planetList() ),
      m_defaultSize( AbstractFloatItem::size() ),
      ui_configWidget( 0 ),
      m_configDialog( 0 ),
      m_mapChanged( false )
{
    // cache is no needed because:
    // (1) the SVG overview map is already rendered and stored in m_worldmap pixmap
    // (2) bounding box and location dot keep changing during navigation
    setCacheMode( NoCache );
    connect( this, SIGNAL( settingsChanged( QString ) ),
             this, SLOT( updateSettings() ) );

    restoreDefaultSettings();
}

OverviewMap::~OverviewMap()
{
}

QStringList OverviewMap::backendTypes() const
{
    return QStringList( "overviewmap" );
}

QString OverviewMap::name() const
{
    return tr("Overview Map");
}

QString OverviewMap::guiString() const
{
    return tr("&Overview Map");
}

QString OverviewMap::nameId() const
{
    return QString( "overviewmap" );
}

QString OverviewMap::version() const
{
    return "1.0";
}

QString OverviewMap::description() const
{
    return tr("This is a float item that provides an overview map.");
}

QString OverviewMap::copyrightYears() const
{
    return "2008";
}

QList<PluginAuthor> OverviewMap::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Torsten Rahn", "tackat@kde.org" );
}

QIcon OverviewMap::icon () const
{
    return QIcon(":/icons/worldmap.png");
}

QDialog *OverviewMap::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::OverviewMapConfigWidget;
        ui_configWidget->setupUi( m_configDialog );
        for( int i = 0; i < m_planetID.size(); ++i ) {
            ui_configWidget->m_planetComboBox->addItem( Planet::name(m_planetID.value( i ) ) );
        }
        ui_configWidget->m_planetComboBox->setCurrentIndex( 2 );
        readSettings();
        loadMapSuggestions();
        connect( ui_configWidget->m_buttonBox, SIGNAL( accepted() ),
                 SLOT( writeSettings() ) );
        connect( ui_configWidget->m_buttonBox, SIGNAL( rejected() ),
                 SLOT( readSettings() ) );
        connect( ui_configWidget->m_buttonBox->button( QDialogButtonBox::Reset ), SIGNAL( clicked () ),
                 SLOT( restoreDefaultSettings() ) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                 SLOT( writeSettings() ) );
        connect( ui_configWidget->m_fileChooserButton, SIGNAL( clicked() ),
                 SLOT( chooseCustomMap() ) );
        connect( ui_configWidget->m_widthBox, SIGNAL( valueChanged( int ) ),
                 SLOT( synchronizeSpinboxes() ) );
        connect( ui_configWidget->m_heightBox, SIGNAL( valueChanged( int ) ),
                 SLOT( synchronizeSpinboxes() ) );
        connect( ui_configWidget->m_planetComboBox, SIGNAL( currentIndexChanged( int ) ),
                 SLOT( showCurrentPlanetPreview() ) );
        connect( ui_configWidget->m_colorChooserButton, SIGNAL( clicked() ),
                 SLOT( choosePositionIndicatorColor() ) );
        connect( ui_configWidget->m_tableWidget, SIGNAL( cellClicked ( int, int ) ),
                 SLOT( useMapSuggestion( int ) ) );
    }
    return m_configDialog;
}

void OverviewMap::initialize ()
{
}

bool OverviewMap::isInitialized () const
{
    return true;
}

void OverviewMap::changeViewport( ViewportParams *viewport )
{
    GeoDataLatLonAltBox latLonAltBox = viewport->latLonAltBox( QRect( QPoint( 0, 0 ), viewport->size() ) );
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();
    QString target = marbleModel()->planetId();

    if ( !( m_latLonAltBox == latLonAltBox
            && m_centerLon == centerLon
            && m_centerLat == centerLat
            && m_target == target ) )
    {
        m_latLonAltBox = latLonAltBox;
        m_centerLon = centerLon;
        m_centerLat = centerLat;
        update();
    }
}

void OverviewMap::paintContent( QPainter *painter )
{
    painter->save();

    QRectF mapRect( contentRect() );

    QString target = marbleModel()->planetId();

    if ( target != m_target ) {
        changeBackground( target );
    }

    if ( m_svgobj.isValid() ) {
        // Rerender worldmap pixmap if the size or map has changed
        if ( m_worldmap.size() != mapRect.size().toSize() 
            || target != m_target || m_mapChanged ) {
            m_mapChanged = false;
            m_worldmap = QPixmap( mapRect.size().toSize() );
            m_worldmap.fill( Qt::transparent );
            QPainter mapPainter;
            mapPainter.begin( &m_worldmap );
            mapPainter.setViewport( m_worldmap.rect() );
            m_svgobj.render( &mapPainter );
            mapPainter.end(); 
        }

        painter->drawPixmap( QPoint( 0, 0 ), m_worldmap );
    }
    else {
        painter->setPen( QPen( Qt::DashLine ) );
        painter->drawRect( QRectF( QPoint( 0, 0 ), mapRect.size().toSize() ) );

        for ( int y = 1; y < 4; ++y ) {
            if ( y == 2 ) {
                painter->setPen( QPen( Qt::DashLine ) );
            }
            else {
                painter->setPen( QPen( Qt::DotLine ) );
            }

            painter->drawLine( 0.0, 0.25 * y * mapRect.height(),
                                mapRect.width(), 0.25 * y * mapRect.height() );
        }
        for ( int x = 1; x < 8; ++x ) {
            if ( x == 4 ) {
                painter->setPen( QPen( Qt::DashLine ) );
            }
            else {
                painter->setPen( QPen( Qt::DotLine ) );
            }

            painter->drawLine( 0.125 * x * mapRect.width(), 0,
                               0.125 * x * mapRect.width(), mapRect.height() );
        }
    }

    m_target = target;

    // Now draw the latitude longitude bounding box
    qreal xWest = mapRect.width() / 2.0 
                    + mapRect.width() / ( 2.0 * M_PI ) * m_latLonAltBox.west();
    qreal xEast = mapRect.width() / 2.0
                    + mapRect.width() / ( 2.0 * M_PI ) * m_latLonAltBox.east();
    qreal xNorth = mapRect.height() / 2.0 
                    - mapRect.height() / M_PI * m_latLonAltBox.north();
    qreal xSouth = mapRect.height() / 2.0
                    - mapRect.height() / M_PI * m_latLonAltBox.south();

    qreal lon = m_centerLon;
    qreal lat = m_centerLat;
    GeoDataPoint::normalizeLonLat( lon, lat );
    qreal x = mapRect.width() / 2.0 + mapRect.width() / ( 2.0 * M_PI ) * lon;
    qreal y = mapRect.height() / 2.0 - mapRect.height() / M_PI * lat;

    painter->setPen( QPen( Qt::white ) );
    painter->setBrush( QBrush( Qt::transparent ) );
    painter->setRenderHint( QPainter::Antialiasing, false );

    qreal boxWidth  = xEast  - xWest;
    qreal boxHeight = xSouth - xNorth;

    qreal minBoxSize = 2.0;
    if ( boxHeight < minBoxSize ) boxHeight = minBoxSize;

    if ( m_latLonAltBox.west() <= m_latLonAltBox.east() ) {
        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;

        painter->drawRect( QRectF( xWest, xNorth, boxWidth, boxHeight ) );
    }
    else {
        // If the dateline is shown in the viewport  and if the poles are not 
        // then there are two boxes that represent the latLonBox of the view.

        boxWidth = xEast;

        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;

        painter->drawRect( QRectF( 0, xNorth, boxWidth, boxHeight ) );

        boxWidth = mapRect.width() - xWest;

        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;

        painter->drawRect( QRectF( xWest, xNorth, boxWidth, boxHeight ) );
    }

    painter->setPen( QPen( m_posColor ) );
    painter->setBrush( QBrush( m_posColor ) );

    qreal circleRadius = 2.5;
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->drawEllipse( QRectF( x - circleRadius, y - circleRadius , 2 * circleRadius, 2 * circleRadius ) );

    painter->restore();
}

QHash<QString,QVariant> OverviewMap::settings() const
{
    return m_settings;
}

void OverviewMap::setSettings( const QHash<QString,QVariant> &settings )
{
    m_settings = settings;

    if( !m_settings.contains( "width" ) ) {
        m_settings.insert( "width", m_defaultSize.toSize().width() );
    }
    if( !m_settings.contains( "height" ) ) {
        m_settings.insert( "height", m_defaultSize.toSize().height() );
    }
    QString const worldmap = MarbleDirs::path( "svg/worldmap.svg" );
    QStringList const planets = Planet::planetList();
    if ( !m_settings.contains( "path_" + m_planetID[2] ) ) {
        m_settings.insert( "path_" + m_planetID[2], worldmap );
    }
    if ( !m_settings.contains( "path_" + m_planetID[10] ) ) {
        m_settings.insert( "path_" + m_planetID[10], MarbleDirs::path( "svg/lunarmap.svg" ) );
    }
    foreach( const QString& planet, planets ) {
        if ( !m_settings.contains( "path_" + planet ) ) {
            QString const planetMap = MarbleDirs::path( QString( "svg/%1map.svg" ).arg( planet ) );
            QString const mapFile = planetMap.isEmpty() ? worldmap : planetMap;
            m_settings.insert( "path_" + planet, mapFile );
        }
    }

    if( !m_settings.contains( "posColor" ) ) {
        m_settings.insert( "posColor", QColor( Qt::white ).name() );
    }

    m_target.clear(); // FIXME: forces execution of changeBackground() in paintContent()

    readSettings();
    emit settingsChanged( nameId() );
}

void OverviewMap::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    ui_configWidget->m_widthBox->setValue( m_settings.value( "width" ).toInt() );
    ui_configWidget->m_heightBox->setValue( m_settings.value( "height" ).toInt() );
    QPalette palette = ui_configWidget->m_colorChooserButton->palette();
    palette.setColor( QPalette::Button, QColor( m_settings.value( "posColor" ).toString() ) );
    ui_configWidget->m_colorChooserButton->setPalette( palette );
}

void OverviewMap::writeSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    m_settings.insert( "width", contentRect().width() );
    m_settings.insert( "height", contentRect().height() );

    QStringList const planets = Planet::planetList();
    foreach( const QString &planet, planets ) {
        m_settings.insert( "path_" + planet, m_svgPaths[planet] );
    }

    m_settings.insert( "posColor", m_posColor.name() );

    emit settingsChanged( nameId() );
}

void OverviewMap::updateSettings()
{
    QStringList const planets = Planet::planetList();
    foreach( const QString &planet, planets ) {
        m_svgPaths.insert( planet, m_settings.value( "path_" + planet, QString() ).toString() );
    }

    m_posColor = QColor( m_settings.value( "posColor" ).toString() );
    loadPlanetMaps();

    if ( !m_configDialog ) {
        return;
    }
    
    setCurrentWidget( m_svgWidgets[m_planetID[2]] );
    showCurrentPlanetPreview();
    setContentSize( QSizeF( ui_configWidget->m_widthBox->value(), ui_configWidget->m_heightBox->value() ) );
}

bool OverviewMap::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*>(object);
    if ( !widget ) {
        return AbstractFloatItem::eventFilter(object,e);
    }

    bool cursorAboveFloatItem(false);
    if ( e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::MouseMove ) {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);
        QRectF floatItemRect = QRectF( positivePosition(), size() );

        if ( floatItemRect.contains(event->pos()) ) {
            cursorAboveFloatItem = true;

            // Double click triggers recentering the map at the specified position
            if ( e->type() == QEvent::MouseButtonDblClick ) {
                QRectF mapRect( contentRect() );
                QPointF pos = event->pos() - floatItemRect.topLeft() 
                    - QPointF(padding(),padding());

                qreal lon = ( pos.x() - mapRect.width() / 2.0 ) / mapRect.width() * 360.0 ;
                qreal lat = ( mapRect.height() / 2.0 - pos.y() ) / mapRect.height() * 180.0;
                widget->centerOn(lon,lat,true);

                return true;
            }
        }

        if ( cursorAboveFloatItem && e->type() == QEvent::MouseMove 
                && !event->buttons() & Qt::LeftButton )
        {
            // Cross hair cursor when moving above the float item without pressing a button
            widget->setCursor(QCursor(Qt::CrossCursor));
            return true;
        }
    }

    return AbstractFloatItem::eventFilter(object,e);
}

void OverviewMap::changeBackground( const QString& target )
{
    m_svgobj.load( m_svgPaths[target] );
}

QSvgWidget *OverviewMap::currentWidget() const
{
    return m_svgWidgets[m_planetID[ui_configWidget->m_planetComboBox->currentIndex()]];
}

void OverviewMap::setCurrentWidget( QSvgWidget *widget )
{
    m_svgWidgets[m_planetID[ui_configWidget->m_planetComboBox->currentIndex()]] = widget;
    m_mapChanged = true;
    if( m_target == m_planetID[ui_configWidget->m_planetComboBox->currentIndex()] ) {
        changeBackground( m_target );
    }
}

void OverviewMap::loadPlanetMaps()
{
    foreach( const QString& planet, m_planetID ) {
        if ( m_svgWidgets.contains( planet) ) {
            m_svgWidgets[planet]->load( m_svgPaths[planet] );
        } else {
            m_svgWidgets[planet] = new QSvgWidget( m_svgPaths[planet], m_configDialog );
        }
    }
}

void OverviewMap::loadMapSuggestions()
{
    QStringList paths = QDir( MarbleDirs::pluginPath( "" ) ).entryList( QStringList( "*.svg" ), QDir::Files | QDir::NoDotAndDotDot );
    for( int i = 0; i < paths.size(); ++i ) {
        paths[i] = MarbleDirs::pluginPath( "" ) + "/" + paths[i];
    }
    paths << MarbleDirs::path( "svg/worldmap.svg" ) << MarbleDirs::path( "svg/lunarmap.svg" );
    ui_configWidget->m_tableWidget->setRowCount( paths.size() );
    for( int i = 0; i < paths.size(); ++i ) {
        ui_configWidget->m_tableWidget->setCellWidget( i, 0, new QSvgWidget( paths[i], m_configDialog ) );
        ui_configWidget->m_tableWidget->setItem( i, 1, new QTableWidgetItem( paths[i] ) );
    }
}

void OverviewMap::chooseCustomMap()
{
    QString path = QFileDialog::getOpenFileName ( m_configDialog, tr( "Choose Overview Map" ), "", "SVG (*.svg)" );
    if( !path.isNull() )
    {
        ui_configWidget->m_fileChooserButton->layout()->removeWidget( currentWidget() );
        delete currentWidget();
        QSvgWidget *widget = new QSvgWidget( path );
        setCurrentWidget( widget );
        ui_configWidget->m_fileChooserButton->layout()->addWidget( widget );
        m_svgPaths[m_planetID[ui_configWidget->m_planetComboBox->currentIndex()]] = path;
    }
}

void OverviewMap::synchronizeSpinboxes()
{
    if( sender() == ui_configWidget->m_widthBox ) {
        ui_configWidget->m_heightBox->setValue( ui_configWidget->m_widthBox->value() / 2 );
    }
    else if( sender() == ui_configWidget->m_heightBox ) {
        ui_configWidget->m_widthBox->setValue( ui_configWidget->m_heightBox->value() * 2 );
    }
}

void OverviewMap::showCurrentPlanetPreview() const
{
    delete ui_configWidget->m_fileChooserButton->layout();
    ui_configWidget->m_fileChooserButton->setLayout( new QHBoxLayout() );
    ui_configWidget->m_fileChooserButton->layout()->addWidget( currentWidget() );
}

void OverviewMap::choosePositionIndicatorColor()
{
    QColor c = QColorDialog::getColor( m_posColor, 0, 
                                       tr( "Please choose the color for the position indicator" ), 
                                       QColorDialog::ShowAlphaChannel );
    if( c.isValid() )
    {
        m_posColor = c;
        QPalette palette = ui_configWidget->m_colorChooserButton->palette();
        palette.setColor( QPalette::Button, m_posColor );
        ui_configWidget->m_colorChooserButton->setPalette( palette );
    }
}

void OverviewMap::useMapSuggestion( int index )
{
    QString path = ui_configWidget->m_tableWidget->item( index, 1 )->text();
    m_svgPaths[m_planetID[ui_configWidget->m_planetComboBox->currentIndex()]] = path;
    delete currentWidget();
    QSvgWidget *widget = new QSvgWidget( path );
    setCurrentWidget( widget );
    showCurrentPlanetPreview();
}

}

Q_EXPORT_PLUGIN2( OverviewMap, Marble::OverviewMap )

#include "OverviewMap.moc"
