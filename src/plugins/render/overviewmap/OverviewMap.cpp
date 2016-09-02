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

#include <QRect>
#include <QStringList>
#include <QCursor>
#include <QMouseEvent>
#include <QPainter>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QColorDialog>

#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "ui_OverviewMapConfigWidget.h"

#include "GeoDataPoint.h"
#include "ViewportParams.h"
#include "MarbleWidget.h"
#include "Planet.h"
#include "PlanetFactory.h"

namespace Marble
{

OverviewMap::OverviewMap()
    : AbstractFloatItem( 0 ),
      ui_configWidget( 0 ),
      m_configDialog( 0 ),
      m_mapChanged( false )
{
}

OverviewMap::OverviewMap( const MarbleModel *marbleModel )
    : AbstractFloatItem( marbleModel, QPointF( 10.5, 10.5 ), QSizeF( 166.0, 86.0 ) ),
      m_target(),
      m_planetID( PlanetFactory::planetList() ),
      m_defaultSize( AbstractFloatItem::size() ),
      ui_configWidget( 0 ),
      m_configDialog( 0 ),
      m_mapChanged( false )
{
    // cache is no needed because:
    // (1) the SVG overview map is already rendered and stored in m_worldmap pixmap
    // (2) bounding box and location dot keep changing during navigation
    setCacheMode( NoCache );
    connect( this, SIGNAL(settingsChanged(QString)),
             this, SLOT(updateSettings()) );

    restoreDefaultSettings();
}

OverviewMap::~OverviewMap()
{
    QHash<QString, QSvgWidget *>::const_iterator pos = m_svgWidgets.constBegin();
    QHash<QString, QSvgWidget *>::const_iterator const end = m_svgWidgets.constEnd();
    for (; pos != end; ++pos ) {
        delete pos.value();
    }
}

QStringList OverviewMap::backendTypes() const
{
    return QStringList(QStringLiteral("overviewmap"));
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
    return QStringLiteral("overviewmap");
}

QString OverviewMap::version() const
{
    return QStringLiteral("1.0");
}

QString OverviewMap::description() const
{
    return tr("This is a float item that provides an overview map.");
}

QString OverviewMap::copyrightYears() const
{
    return QStringLiteral("2008");
}

QVector<PluginAuthor> OverviewMap::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Torsten Rahn"), QStringLiteral("tackat@kde.org"));
}

QIcon OverviewMap::icon () const
{
    return QIcon(QStringLiteral(":/icons/worldmap.png"));
}

QDialog *OverviewMap::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::OverviewMapConfigWidget;
        ui_configWidget->setupUi( m_configDialog );
        for( int i = 0; i < m_planetID.size(); ++i ) {
            ui_configWidget->m_planetComboBox->addItem( PlanetFactory::localizedName(m_planetID.value( i ) ) );
        }
        ui_configWidget->m_planetComboBox->setCurrentIndex( 2 );
        readSettings();
        loadMapSuggestions();
        connect( ui_configWidget->m_buttonBox, SIGNAL(accepted()),
                 SLOT(writeSettings()) );
        connect( ui_configWidget->m_buttonBox, SIGNAL(rejected()),
                 SLOT(readSettings()) );
        connect( ui_configWidget->m_buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()),
                 SLOT(restoreDefaultSettings()) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()),
                 SLOT(writeSettings()) );
        connect( ui_configWidget->m_fileChooserButton, SIGNAL(clicked()),
                 SLOT(chooseCustomMap()) );
        connect( ui_configWidget->m_widthBox, SIGNAL(valueChanged(int)),
                 SLOT(synchronizeSpinboxes()) );
        connect( ui_configWidget->m_heightBox, SIGNAL(valueChanged(int)),
                 SLOT(synchronizeSpinboxes()) );
        connect( ui_configWidget->m_planetComboBox, SIGNAL(currentIndexChanged(int)),
                 SLOT(showCurrentPlanetPreview()) );
        connect( ui_configWidget->m_colorChooserButton, SIGNAL(clicked()),
                 SLOT(choosePositionIndicatorColor()) );
        connect( ui_configWidget->m_tableWidget, SIGNAL(cellClicked(int,int)),
                 SLOT(useMapSuggestion(int)) );
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

void OverviewMap::setProjection( const ViewportParams *viewport )
{
    GeoDataLatLonAltBox latLonAltBox = viewport->latLonAltBox( QRect( QPoint( 0, 0 ), viewport->size() ) );
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();
    QString target = marbleModel()->planetId();

    if ( target != m_target ) {
        changeBackground( target );
        m_target = target;
        update();
    }

    if ( !( m_latLonAltBox == latLonAltBox
            && m_centerLon == centerLon
            && m_centerLat == centerLat ) )
    {
        m_latLonAltBox = latLonAltBox;
        m_centerLon = centerLon;
        m_centerLat = centerLat;
        update();
    }

    AbstractFloatItem::setProjection( viewport );
}

void OverviewMap::paintContent( QPainter *painter )
{
    painter->save();

    QRectF mapRect( contentRect() );

    if ( m_svgobj.isValid() ) {
        // Rerender worldmap pixmap if the size or map has changed
        if ( m_worldmap.size() != mapRect.size().toSize() || m_mapChanged ) {
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
    GeoDataCoordinates::normalizeLonLat( lon, lat );
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
    QHash<QString, QVariant> result = AbstractFloatItem::settings();

    typedef QHash<QString, QVariant>::ConstIterator Iterator;
    Iterator end = m_settings.constEnd();
    for ( Iterator iter = m_settings.constBegin(); iter != end; ++iter ) {
        result.insert( iter.key(), iter.value() );
    }

    return result;
}

void OverviewMap::setSettings( const QHash<QString,QVariant> &settings )
{
    AbstractFloatItem::setSettings( settings );

    m_settings.insert(QStringLiteral("width"), settings.value(QStringLiteral("width"), m_defaultSize.toSize().width()));
    m_settings.insert(QStringLiteral("height"), settings.value(QStringLiteral("height"), m_defaultSize.toSize().height()));

    foreach ( const QString& planet, PlanetFactory::planetList() ) {
        QString mapFile = MarbleDirs::path(QLatin1String("svg/") + planet + QLatin1String("map.svg"));

        if (planet == QLatin1String("moon")) {
            mapFile = MarbleDirs::path(QStringLiteral("svg/lunarmap.svg"));
        }
        else if (planet == QLatin1String("earth") || mapFile.isEmpty()) {
            mapFile = MarbleDirs::path(QStringLiteral("svg/worldmap.svg"));
        }

        const QString id = QLatin1String("path_") + planet;
        m_settings.insert(id, settings.value(id, mapFile));
    }

    m_settings.insert(QStringLiteral("posColor"), settings.value(QStringLiteral("posColor"), QColor(Qt::white).name()));

    m_target.clear(); // FIXME: forces execution of changeBackground() in changeViewport()

    readSettings();
    emit settingsChanged( nameId() );
}

void OverviewMap::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    ui_configWidget->m_widthBox->setValue( m_settings.value(QStringLiteral("width")).toInt() );
    ui_configWidget->m_heightBox->setValue( m_settings.value(QStringLiteral("height")).toInt() );
    QPalette palette = ui_configWidget->m_colorChooserButton->palette();
    palette.setColor(QPalette::Button, QColor(m_settings.value(QStringLiteral("posColor")).toString()));
    ui_configWidget->m_colorChooserButton->setPalette( palette );
}

void OverviewMap::writeSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    m_settings.insert(QStringLiteral("width"), contentRect().width());
    m_settings.insert(QStringLiteral("height"), contentRect().height());

    QStringList const planets = PlanetFactory::planetList();
    foreach( const QString &planet, planets ) {
        m_settings.insert(QLatin1String("path_") + planet, m_svgPaths[planet]);
    }

    m_settings.insert(QStringLiteral("posColor"), m_posColor.name());

    emit settingsChanged( nameId() );
}

void OverviewMap::updateSettings()
{
    QStringList const planets = PlanetFactory::planetList();
    foreach( const QString &planet, planets ) {
        m_svgPaths.insert(planet, m_settings.value(QLatin1String("path_") + planet, QString()).toString());
    }

    m_posColor = QColor(m_settings.value(QStringLiteral("posColor")).toString());
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

    if ( e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::MouseMove ) {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);
        QRectF floatItemRect = QRectF( positivePosition(), size() );

        bool cursorAboveFloatItem(false);
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
                && !(event->buttons() & Qt::LeftButton) )
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
    m_mapChanged = true;
}

QSvgWidget *OverviewMap::currentWidget() const
{
    return m_svgWidgets[m_planetID[ui_configWidget->m_planetComboBox->currentIndex()]];
}

void OverviewMap::setCurrentWidget( QSvgWidget *widget )
{
    m_svgWidgets[m_planetID[ui_configWidget->m_planetComboBox->currentIndex()]] = widget;
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
            m_svgWidgets[planet] = new QSvgWidget( m_svgPaths[planet] );
        }
    }
}

void OverviewMap::loadMapSuggestions()
{
    QStringList paths = QDir(MarbleDirs::pluginPath(QString())).entryList(QStringList("*.svg"), QDir::Files | QDir::NoDotAndDotDot);
    for( int i = 0; i < paths.size(); ++i ) {
        paths[i] = MarbleDirs::pluginPath(QString()) + QLatin1Char('/') + paths[i];
    }
    paths << MarbleDirs::path(QStringLiteral("svg/worldmap.svg")) << MarbleDirs::path(QStringLiteral("svg/lunarmap.svg"));
    ui_configWidget->m_tableWidget->setRowCount( paths.size() );
    for( int i = 0; i < paths.size(); ++i ) {
        ui_configWidget->m_tableWidget->setCellWidget( i, 0, new QSvgWidget( paths[i] ) );
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

#include "moc_OverviewMap.cpp"
