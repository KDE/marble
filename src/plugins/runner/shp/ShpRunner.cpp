//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "ShpRunner.h"

#include "GeoDataDocument.h"
#include "GeoDataPolygon.h"
#include "MarbleDebug.h"

#include <QtCore/QFileInfo>

#include <shapefil.h>

namespace Marble
{

ShpRunner::ShpRunner(QObject *parent) :
    MarbleAbstractRunner(parent)
{
}

ShpRunner::~ShpRunner()
{
}

GeoDataFeature::GeoDataVisualCategory ShpRunner::category() const
{
    return GeoDataFeature::Folder;
}

void ShpRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFileInfo fileinfo( fileName );
    if( fileinfo.suffix().compare( "shp", Qt::CaseInsensitive ) != 0 ) {
        emit parsingFinished( 0 );
        return;
    }

    SHPHandle handle = SHPOpen( fileName.toStdString().c_str(), "rb" );
    if ( !handle ) {
        emit parsingFinished( 0 );
        return;
    }
    int entities;
    int shapeType;
    SHPGetInfo( handle, &entities, &shapeType, NULL, NULL );
    mDebug() << " SHP info " << entities << " Entities "
             << shapeType << " Shape Type ";

    DBFHandle dbfhandle;
    dbfhandle = DBFOpen( fileName.toStdString().c_str(), "rb");
    int nameField = DBFGetFieldIndex( dbfhandle, "Name" );
    int noteField = DBFGetFieldIndex( dbfhandle, "Note" );

    GeoDataDocument *document = new GeoDataDocument;
    document->setDocumentRole( role );

    for ( int i=0; i< entities; ++i ) {
        GeoDataPlacemark  *placemark = 0;
        placemark = new GeoDataPlacemark;
        document->append( placemark );

        SHPObject *shape = SHPReadObject( handle, i );
        if( nameField ) {
            const char* info = DBFReadStringAttribute( dbfhandle, i, nameField );
            placemark->setName( info );
            mDebug() << "name " << placemark->name();
        }
        if( noteField ) {
            const char* note = DBFReadStringAttribute( dbfhandle, i, noteField );
            placemark->setDescription( note );
            mDebug() << "desc " << placemark->description();
        }

        switch ( shapeType ) {
            case SHPT_POINT: {
                placemark->setCoordinate( *shape->padfX, *shape->padfY,
                                         0, GeoDataCoordinates::Degree );
                mDebug() << "point " << placemark->name();
                break;
            }

            case SHPT_MULTIPOINT: {
                GeoDataMultiGeometry *geom = new GeoDataMultiGeometry;
                for( int j=0; j<shape->nVertices; ++j ) {
                    geom->append( new GeoDataPoint( GeoDataCoordinates(
                                  shape->padfX[j], shape->padfY[j],
                                  0, GeoDataCoordinates::Degree ) ) );
                }
                placemark->setGeometry( geom );
                mDebug() << "multipoint " << placemark->name();
                break;
            }

            case SHPT_ARC: {
                if ( shape->nParts != 1 ) {
                    GeoDataMultiGeometry *geom = new GeoDataMultiGeometry;
                    for( int j=0; j<shape->nParts-1; ++j ) {
                        GeoDataLineString *line = new GeoDataLineString;
                        for( int k=shape->panPartStart[j]; k<shape->panPartStart[j+1]; ++k ) {
                            line->append( GeoDataCoordinates(
                                          shape->padfX[k], shape->padfY[k],
                                          0, GeoDataCoordinates::Degree ) );
                        }
                        geom->append( line );
                    }
                    placemark->setGeometry( geom );
                    mDebug() << "arc " << placemark->name() << " " << shape->nParts;

                } else {
                    GeoDataLineString *line = new GeoDataLineString;
                    for( int j=0; j<shape->nVertices; ++j ) {
                        line->append( GeoDataCoordinates(
                                      shape->padfX[j], shape->padfY[j],
                                      0, GeoDataCoordinates::Degree ) );
                    }
                    placemark->setGeometry( line );
                    mDebug() << "arc " << placemark->name() << " " << shape->nParts;
                }
                break;
            }

            case SHPT_POLYGON: {
                if ( shape->nParts != 1 ) {
                    GeoDataPolygon *poly = new GeoDataPolygon;
                    for( int j=0; j<shape->nParts-1; ++j ) {
                        GeoDataLinearRing ring;
                        for( int k=shape->panPartStart[j]; k<shape->panPartStart[j+1]; ++k ) {
                            ring.append( GeoDataCoordinates(
                                         shape->padfX[k], shape->padfY[k],
                                         0, GeoDataCoordinates::Degree ) );
                        }
                        // TODO: outer boundary per SHP spec is for the clockwise ring
                        // and inner holes are anticlockwise
                        if ( j==0 ) {
                            poly->setOuterBoundary( ring );
                        } else {
                            poly->appendInnerBoundary( ring );
                        }
                    }
                    placemark->setGeometry( poly );
                    mDebug() << "donut " << placemark->name() << " " << shape->nParts;

                } else {
                    GeoDataPolygon *poly = new GeoDataPolygon;
                    GeoDataLinearRing ring;
                    for( int j=0; j<shape->nVertices; ++j ) {
                        ring.append( GeoDataCoordinates(
                                         shape->padfX[j], shape->padfY[j],
                                         0, GeoDataCoordinates::Degree ) );
                    }
                    poly->setOuterBoundary( ring );
                    placemark->setGeometry( poly );
                    mDebug() << "poly " << placemark->name() << " " << shape->nParts;
                }
                break;
            }
        }

    }

    SHPClose( handle );

    DBFClose( dbfhandle );

    if ( document->size() ) {
        emit parsingFinished( document );
    } else {
        emit parsingFinished( 0 );
    }
}

}

#include "ShpRunner.moc"
