//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
//

#include "ElevationProfileMarker.h"

#include <QtCore/QRect>

#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoPainter.h"
#include "GeoDataTreeModel.h"
#include "ViewportParams.h"
#include "MarbleGraphicsGridLayout.h"

namespace Marble
{

ElevationProfileMarker::ElevationProfileMarker()
    : RenderPlugin( 0 )
{
}

ElevationProfileMarker::ElevationProfileMarker( const MarbleModel *marbleModel )
        : RenderPlugin( marbleModel ),
        m_fontHeight( 10 ),
        m_markerPlacemark( 0 ),
        m_markerItem(),
        m_markerIcon( &m_markerItem ),
        m_markerText( &m_markerItem )
{
    setVisible( false );
    m_markerItem.setCacheMode( MarbleGraphicsItem::ItemCoordinateCache );

    connect( const_cast<MarbleModel *>( marbleModel )->treeModel(), SIGNAL( added( GeoDataObject * ) ),
             this, SLOT( onGeoObjectAdded( GeoDataObject * ) ) );
    connect( const_cast<MarbleModel *>( marbleModel )->treeModel(), SIGNAL( removed( GeoDataObject * ) ),
             this, SLOT( onGeoObjectRemoved( GeoDataObject * ) ) );
}

ElevationProfileMarker::~ElevationProfileMarker()
{
}

QStringList ElevationProfileMarker::backendTypes() const
{
    return QStringList( "elevationprofilemarker" );
}

QString ElevationProfileMarker::renderPolicy() const
{
    return "ALWAYS";
}

QStringList ElevationProfileMarker::renderPosition() const
{
    return QStringList() << "HOVERS_ABOVE_SURFACE";
}

qreal ElevationProfileMarker::zValue() const
{
    return 3.0;
}

QString ElevationProfileMarker::name() const
{
    return tr( "Elevation Profile Marker" );
}

QString ElevationProfileMarker::guiString() const
{
    return tr( "&Elevation Profile Marker" );
}

QString ElevationProfileMarker::nameId() const
{
    return QString( "elevationprofilemarker" );
}

QString ElevationProfileMarker::version() const
{
    return "1.0";
}

QString ElevationProfileMarker::description() const
{
    return tr( "Marks the current elevation of the elevation profile on the map." );
}

QString ElevationProfileMarker::copyrightYears() const
{
    return "2011, 2012";
}

QList<PluginAuthor> ElevationProfileMarker::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" )
            << PluginAuthor( QString::fromUtf8 ( "Florian Eßer" ), "f.esser@rwth-aachen.de" );
}

QIcon ElevationProfileMarker::icon() const
{
    return QIcon(":/icons/elevationprofile.png");
}

void ElevationProfileMarker::initialize()
{
    m_markerIcon.setImage( QImage( ":/flag-red-mirrored.png" ) );

    MarbleGraphicsGridLayout *topLayout = new MarbleGraphicsGridLayout( 1, 2 );
    m_markerItem.setLayout( topLayout );
    topLayout->addItem( &m_markerIcon, 0, 0 );

    m_markerText.setFrame( LabelGraphicsItem::RoundedRectFrame );
    m_markerText.setPadding( 1 );
    topLayout->addItem( &m_markerText, 0, 1 );
}

bool ElevationProfileMarker::isInitialized() const
{
    return !m_markerIcon.image().isNull();
}

bool ElevationProfileMarker::render( GeoPainter* painter, ViewportParams* viewport, const QString& renderPos, GeoSceneLayer* layer )
{
    if ( !m_markerPlacemark )
        return true;

    if ( m_currentPosition != m_markerPlacemark->coordinate() ) {
        m_currentPosition = m_markerPlacemark->coordinate();

        if ( m_currentPosition.isValid() ) {
            QString unitString = tr( "m" );
            int displayScale = 1.0;
            const QLocale::MeasurementSystem measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();
            switch ( measurementSystem ) {
            case QLocale::MetricSystem:
                /* nothing to do */
                break;
            case QLocale::ImperialSystem:
                unitString = tr( "ft" );
                displayScale = M2FT;
                break;
            }

            QString intervalStr;
            intervalStr.setNum( m_currentPosition.altitude() * displayScale, 'f', 1 );
            intervalStr += " " + unitString;
            m_markerText.setText( intervalStr );
        }
    }

    if ( m_currentPosition.isValid() ) {
        qreal x;
        qreal y;
        qreal lon;
        qreal lat;
        // move the icon by some pixels, so that the pole of the flag sits at the exact point
        int dx = +3 + m_markerItem.size().width() / 2 - m_markerIcon.contentRect().right();//-4;
        int dy = -6;
        viewport->screenCoordinates( m_currentPosition.longitude( GeoDataCoordinates::Radian ),
                                     m_currentPosition.latitude ( GeoDataCoordinates::Radian ),
                                     x, y );
        viewport->geoCoordinates( x + dx, y + dy, lon, lat, GeoDataCoordinates::Radian );
        m_markerItem.setCoordinate( GeoDataCoordinates( lon, lat, m_currentPosition.altitude(),
                                                            GeoDataCoordinates::Radian ) );

        painter->save();

        m_markerItem.paintEvent( painter, viewport, renderPos, layer );

        painter->restore();
    }

    return true;
}

void ElevationProfileMarker::onGeoObjectAdded( GeoDataObject *object )
{
    if ( m_markerPlacemark )
        return;

    GeoDataDocument *document = dynamic_cast<GeoDataDocument *>( object );

    if ( !document )
        return;

    if ( document->name() != "Elevation Profile" )
        return;

    if ( document->size() < 1 )
        return;

    m_markerPlacemark = dynamic_cast<GeoDataPlacemark *>( document->child( 0 ) );

    setVisible( m_markerPlacemark != 0 );
}

void ElevationProfileMarker::onGeoObjectRemoved( GeoDataObject *object )
{
    GeoDataDocument *const document = dynamic_cast<GeoDataDocument *>( object );
    if ( !document )
        return;

    if ( document->name() != "Elevation Profile" )
        return;

    m_markerPlacemark = 0;

    emit repaintNeeded();
}

}

Q_EXPORT_PLUGIN2(ElevationProfileMarker, Marble::ElevationProfileMarker)

#include "ElevationProfileMarker.moc"
