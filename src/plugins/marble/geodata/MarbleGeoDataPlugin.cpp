//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleGeoDataPlugin.h"

#include <QtCore/QDebug>
#include <QtGui/QColor>
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


QStringList MarbleGeoDataPlugin::backendTypes() const
{
    return QStringList( "geodata" );
}

QString MarbleGeoDataPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList MarbleGeoDataPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString MarbleGeoDataPlugin::name() const
{
    return tr( "GeoData Plugin" );
}

QString MarbleGeoDataPlugin::guiString() const
{
    return tr( "&GeoData Plugin" );
}

QString MarbleGeoDataPlugin::nameId() const
{
    return QString( "GeoData-plugin" );
}

QString MarbleGeoDataPlugin::description() const
{
    return tr( "This is a simple test plugin." );
}

QIcon MarbleGeoDataPlugin::icon () const
{
    return QIcon();
}


void MarbleGeoDataPlugin::initialize ()
{
    dataFacade()->geoDataModel()->addGeoDataFile( GEODATA_DATA_PATH "/germany.kml" );
    dataFacade()->geoDataModel()->addGeoDataFile( GEODATA_DATA_PATH "/poland.kml" );
    dataFacade()->geoDataModel()->addGeoDataFile( GEODATA_DATA_PATH "/czech.kml" );
    m_currentColor = QColor( 0xff, 0x0, 0x0 );
}

bool MarbleGeoDataPlugin::isInitialized () const
{
    return true;
}

bool MarbleGeoDataPlugin::renderGeoDataGeometry( GeoPainter *painter, GeoDataGeometry *object, QString styleUrl )
{
    GeoDataDocument* root = dataFacade()->geoDataModel()->geoDataRoot();
/// hard coded to use only the "normal" style
    QString mapped = root->styleMap( styleUrl.remove( '#' ) )->value( QString( "normal" ) );
    mapped.remove('#');

    if( object->geometryId() == GeoDataPolygonId ) {
        if( m_currentColor != root->style( mapped )->polyStyle()->color() ) {
            m_currentColor = root->style( mapped )->polyStyle()->color();
            painter->setBrush( m_currentColor );
        };
        painter->drawPolygon( dynamic_cast<GeoDataPolygon*>( object )->outerBoundary() );
    }
    if( object->geometryId() == GeoDataLinearRingId ) {
        painter->drawPolyline( *dynamic_cast<GeoDataLinearRing*>( object ) );
    }

    return true;
}

bool MarbleGeoDataPlugin::renderGeoDataFeature( GeoPainter *painter, GeoDataFeature *feature )
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

bool MarbleGeoDataPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    GeoDataDocument* rootDoc = dataFacade()->geoDataModel()->geoDataRoot();
    renderGeoDataFeature( painter, dynamic_cast<GeoDataFeature*>( rootDoc ) );
    return true;
}

Q_EXPORT_PLUGIN2(MarbleGeoDataPlugin, MarbleGeoDataPlugin)

#include "MarbleGeoDataPlugin.moc"
