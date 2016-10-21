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
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataSchema.h"
#include "GeoDataSimpleField.h"
#include "GeoDataStyle.h"
#include "GeoDataPolyStyle.h"
#include "MarbleDebug.h"

#include <QFileInfo>

#include <shapefil.h>

namespace Marble
{

ShpRunner::ShpRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

ShpRunner::~ShpRunner()
{
}

GeoDataDocument *ShpRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    QFileInfo fileinfo( fileName );
    if (fileinfo.suffix().compare(QLatin1String("shp"), Qt::CaseInsensitive) != 0) {
        error = QStringLiteral("File %1 does not have a shp suffix").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    SHPHandle handle = SHPOpen( fileName.toStdString().c_str(), "rb" );
    if ( !handle ) {
        error = QStringLiteral("Failed to read %1").arg(fileName);
        mDebug() << error;
        return nullptr;
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
    int mapColorField = DBFGetFieldIndex( dbfhandle, "mapcolor13" );

    GeoDataDocument *document = new GeoDataDocument;
    document->setDocumentRole( role );

    if ( mapColorField != -1 ) {
        GeoDataSchema schema;
        schema.setId(QStringLiteral("default"));
        GeoDataSimpleField simpleField;
        simpleField.setName(QStringLiteral("mapcolor13"));
        simpleField.setType( GeoDataSimpleField::Double );
        schema.addSimpleField( simpleField );
        document->addSchema( schema );
    }

    for ( int i=0; i< entities; ++i ) {
        GeoDataPlacemark  *placemark = 0;
        placemark = new GeoDataPlacemark;
        document->append( placemark );

        SHPObject *shape = SHPReadObject( handle, i );
        if (nameField != -1) {
            const char* info = DBFReadStringAttribute( dbfhandle, i, nameField );
            // TODO: defaults to utf-8 encoding, but could be also something else, optionally noted in a .cpg file
            placemark->setName( info );
            mDebug() << "name " << placemark->name();
        }
        if (noteField != -1) {
            const char* note = DBFReadStringAttribute( dbfhandle, i, noteField );
            // TODO: defaults to utf-8 encoding, see comment for name
            placemark->setDescription( note );
            mDebug() << "desc " << placemark->description();
        }

        double mapColor = DBFReadDoubleAttribute( dbfhandle, i, mapColorField );
        if ( mapColor ) {
            GeoDataStyle::Ptr style(new GeoDataStyle);
            if ( mapColor >= 0 && mapColor <=255 ) {
                quint8 colorIndex = quint8( mapColor );
                style->polyStyle().setColorIndex( colorIndex );
            }
            else {
                quint8 colorIndex = 0;     // mapColor is undefined in this case
                style->polyStyle().setColorIndex( colorIndex );
            }
            placemark->setStyle( style );
        }

        switch ( shapeType ) {
            case SHPT_POINT: {
                GeoDataPoint *point = new GeoDataPoint( *shape->padfX, *shape->padfY, 0, GeoDataCoordinates::Degree );
                placemark->setGeometry( point );
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
                    for( int j=0; j<shape->nParts; ++j ) {
                        GeoDataLineString *line = new GeoDataLineString;
                        int itEnd = (j + 1 < shape->nParts) ? shape->panPartStart[j+1] : shape->nVertices;
                        for( int k=shape->panPartStart[j]; k<itEnd; ++k ) {
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
                    bool isRingClockwise = false;
                    GeoDataMultiGeometry *multigeom = new GeoDataMultiGeometry;
                    GeoDataPolygon *poly = 0;
                    int polygonCount = 0;
                    for( int j=0; j<shape->nParts; ++j ) {
                        GeoDataLinearRing ring;
                        int itStart = shape->panPartStart[j];
                        int itEnd = (j + 1 < shape->nParts) ? shape->panPartStart[j+1] : shape->nVertices;
                        for( int k = itStart; k<itEnd; ++k ) {
                            ring.append( GeoDataCoordinates(
                                         shape->padfX[k], shape->padfY[k],
                                         0, GeoDataCoordinates::Degree ) );
                        }
                        isRingClockwise = ring.isClockwise();
                        if ( j == 0 || isRingClockwise ) {
                            poly = new GeoDataPolygon;
                            ++polygonCount;
                            poly->setOuterBoundary( ring );
                            if ( polygonCount > 1 ) {
                                multigeom->append( poly );
                            }
                        }
                        else {
                            poly->appendInnerBoundary( ring );
                        }
                    }
                    if ( polygonCount > 1 ) {
                        placemark->setGeometry( multigeom );
                    }
                    else {
                        placemark->setGeometry( poly );
                        delete multigeom;
                        multigeom = 0;
                    }
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
        document->setFileName( fileName );
        return document;
    } else {
        delete document;
        return nullptr;
    }
}

}

#include "moc_ShpRunner.cpp"
