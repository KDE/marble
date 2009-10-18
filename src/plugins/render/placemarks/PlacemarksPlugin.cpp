//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//  Copyright 2008 Simon Schmeisser <mail_to_wrt@gmx.de>
//

#include "PlacemarksPlugin.h"

#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>

#include "MarbleDirs.h"
#include "MarbleDataFacade.h"
#include "MarbleGeoDataModel.h"
#include "GeoPainter.h"

#include "GeoDataCoordinates.h"
#include "GeoDataPolygon.h"
#include "GeoDataPlacemark.h"
#include "GeoDataFeature.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataParser.h"

namespace Marble
{

PlacemarksPlugin::PlacemarksPlugin()
    : m_isInitialized( false )
{
}
    
QStringList PlacemarksPlugin::backendTypes() const
{
    return QStringList( "geodata" );
}

QString PlacemarksPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList PlacemarksPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString PlacemarksPlugin::name() const
{
    return tr( "Placemarks Plugin" );
}

QString PlacemarksPlugin::guiString() const
{
    return tr( "&Placemarks Plugin" );
}

QString PlacemarksPlugin::nameId() const
{
    return QString( "Placemarks-plugin" );
}

QString PlacemarksPlugin::description() const
{
    return tr( "A plugin that displays placemarks." );
}

QIcon PlacemarksPlugin::icon () const
{
    return QIcon();
}


void PlacemarksPlugin::initialize ()
{
    dataFacade()->geoDataModel()->addGeoDataFile( PLACEMARKS_DATA_PATH "/jakobsweg.kml" );
    m_currentBrush = QColor( 0xff, 0x0, 0x0 );
    m_currentPen = QColor( 0xff, 0x0, 0x0 );
    m_initialized = true;
}

bool PlacemarksPlugin::isInitialized () const
{
    return m_initialized;
}

void PlacemarksPlugin::setBrushStyle( GeoPainter *painter, GeoDataDocument* root, QString mapped )
{
    if( root->style( mapped ) && root->style( mapped )->polyStyle() ) {
        if( m_currentBrush.color() != root->style( mapped )->polyStyle()->color() ) {
            qDebug() << "BrushColor:" << root->style( mapped )->polyStyle()->color() << m_currentBrush.color();
            m_currentBrush.setColor( root->style( mapped )->polyStyle()->color() );
            painter->setBrush( m_currentBrush );
        }
    }
}

void PlacemarksPlugin::setPenStyle( GeoPainter *painter, GeoDataDocument* root, QString mapped )
{
    if( root->style( mapped ) && root->style( mapped )->lineStyle() ) {
        if( m_currentPen.color() != root->style( mapped )->lineStyle()->color() || 
            m_currentPen.widthF() != root->style( mapped )->lineStyle()->width() ) {
            qDebug() << "PenColor:" << root->style( mapped )->lineStyle()->color() << m_currentPen.color();
            qDebug() << "PenWidth:" << root->style( mapped )->lineStyle()->width() << m_currentPen.widthF();
            m_currentPen.setColor( root->style( mapped )->lineStyle()->color() );
            m_currentPen.setWidthF( root->style( mapped )->lineStyle()->width() );
            painter->setPen( m_currentPen );
        }
    }
}

bool PlacemarksPlugin::renderGeoDataGeometry( GeoPainter *painter, GeoDataGeometry *object, QString styleUrl )
{
    painter->save();
    painter->autoMapQuality();

    GeoDataDocument* root = dataFacade()->geoDataModel()->geoDataRoot();
/// hard coded to use only the "normal" style
    QString mapped = styleUrl;
    GeoDataStyleMap* styleMap = root->styleMap( styleUrl.remove( '#' ) );
    if( styleMap ) {
        mapped = styleMap->value( QString( "normal" ) );
    }
    mapped.remove( '#' );


    if( object->geometryId() == GeoDataPointId ) {
        setPenStyle( painter, root, mapped );
        painter->drawPoint( *(dynamic_cast<GeoDataPoint*>( object )) );
    }
    painter->restore();
    return true;
}

bool PlacemarksPlugin::renderGeoDataFeature( GeoPainter *painter, GeoDataFeature *feature )
{
    if( !feature ) return false;
    
    if( feature->featureId() == GeoDataDocumentId || feature->featureId() == GeoDataFolderId ) {
        Q_FOREACH( GeoDataFeature *childFeature, static_cast<GeoDataContainer*>(feature)->features() ) {
            renderGeoDataFeature( painter, childFeature );
        }
    }
    if( feature->featureId() == GeoDataPlacemarkId ) {
        renderGeoDataGeometry( painter, dynamic_cast<GeoDataPlacemark*>(feature)->geometry(), dynamic_cast<GeoDataPlacemark*>(feature)->styleUrl() );
    }
    return true;
}

bool PlacemarksPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    GeoDataDocument* rootDoc = dataFacade()->geoDataModel()->geoDataRoot();
    renderGeoDataFeature( painter, dynamic_cast<GeoDataFeature*>( rootDoc ) );
    return true;
}

}

Q_EXPORT_PLUGIN2( PlacemarksPlugin, Marble::PlacemarksPlugin )

#include "PlacemarksPlugin.moc"
