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

MarbleGeoDataPlugin::~MarbleGeoDataPlugin()
{
    delete( m_view );
}

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

//    dataFacade()->geoDataModel()->addGeoDataFile( GEODATA_DATA_PATH "/europe.kml" );
//    dataFacade()->geoDataModel()->addGeoDataFile( GEODATA_DATA_PATH "/germany.kml" );
//    dataFacade()->geoDataModel()->addGeoDataFile( GEODATA_DATA_PATH "/poland.kml" );
//    dataFacade()->geoDataModel()->addGeoDataFile( GEODATA_DATA_PATH "/czech.kml" );
//    dataFacade()->geoDataModel()->addGeoDataFile( GEODATA_DATA_PATH "/Rabet.kml" );

//    GeoDataDocument* rootDoc = dataFacade()->geoDataModel()->geoDataRoot();
    
    m_view = new MarbleGeoDataView();

/*    m_currentBrush = QColor( 0xff, 0x0, 0x0 );
    m_currentPen = QColor( 0xff, 0x0, 0x0 );*/
}

bool MarbleGeoDataPlugin::isInitialized () const
{
    return true;
}
/*
void MarbleGeoDataPlugin::setBrushStyle( GeoPainter *painter, GeoDataDocument* root, QString mapped )
{
    if( root->style( mapped ) && root->style( mapped )->polyStyle() ) {
        if( m_currentBrush.color() != root->style( mapped )->polyStyle()->color() ) {
            qDebug() << "BrushColor:" << root->style( mapped )->polyStyle()->color() << m_currentBrush.color();
            m_currentBrush.setColor( root->style( mapped )->polyStyle()->color() );
            painter->setBrush( m_currentBrush );
        }
    }
}

void MarbleGeoDataPlugin::setPenStyle( GeoPainter *painter, GeoDataDocument* root, QString mapped )
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

bool MarbleGeoDataPlugin::renderGeoDataGeometry( GeoPainter *painter, GeoDataGeometry *object, QString styleUrl )
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


    if( object->geometryId() == GeoDataPolygonId ) {
        setBrushStyle( painter, root, mapped );
        setPenStyle( painter, root, mapped );
        painter->drawPolygon( dynamic_cast<GeoDataPolygon*>( object )->outerBoundary() );
    }
    if( object->geometryId() == GeoDataLinearRingId ) {
        painter->setBrush( QColor( 0, 0, 0, 0 ) );
        setPenStyle( painter, root, mapped );
        painter->drawPolygon( *dynamic_cast<GeoDataLinearRing*>( object ) );
    }
    if( object->geometryId() == GeoDataLineStringId ) {
        setPenStyle( painter, root, mapped );
        painter->drawPolyline( *dynamic_cast<GeoDataLineString*>( object ) );
    }
    if( object->geometryId() == GeoDataMultiGeometryId ) {
        painter->restore();
        
        painter->save();
    }
    painter->restore();
    return true;
}

bool MarbleGeoDataPlugin::renderGeoDataFeature( GeoPainter *painter, GeoDataFeature *feature )
{
    if( !feature ) return false;
    
    if( feature->featureId() == GeoDataDocumentId || feature->featureId() == GeoDataFolderId ) {
        Q_FOREACH( GeoDataFeature *childFeature, static_cast<GeoDataContainer*>( feature )->features() ) {
            renderGeoDataFeature( painter, childFeature );
        }
    }
    if( feature->featureId() == GeoDataPlacemarkId ) {
        renderGeoDataGeometry( painter, dynamic_cast<GeoDataPlacemark*>( feature )->geometry(), dynamic_cast<GeoDataPlacemark*>( feature )->styleUrl() );
    }
    return true;
}
*/
bool MarbleGeoDataPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
//    GeoDataDocument* rootDoc = dataFacade()->geoDataModel()->geoDataRoot();

//    renderGeoDataFeature( painter, dynamic_cast<GeoDataFeature*>( rootDoc ) );
    if( !m_view->model() ) m_view->setModel( dataFacade()->renderModel() );
    m_view->setGeoPainter( painter );
    return true;
}

}

Q_EXPORT_PLUGIN2( MarbleGeoDataPlugin, Marble::MarbleGeoDataPlugin )

#include "MarbleGeoDataPlugin.moc"
