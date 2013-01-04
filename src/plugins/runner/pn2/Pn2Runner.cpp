//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
//
// For the Natural Earth Layer providing the Default data set at 0.5 arcminute resolution should be enough. 
// This fileformat allows for even better packed data than the PNT format. For detailed polygons at arcminute 
// scale on average it should use only 33% of the amount used by PNT.
//
// Description of the file format
//
// In the fileformat initially a file header is provided that provides the file format version and the number 
// of polygons stored inside the file. A Polygon starts with the Polygon Header which provides the feature id 
// and the number of so called "absolute nodes" that are about to follow. Absolute nodes always contain 
// absolute geodetic coordinates. The Polygon Header also provides a flag that allows to specify whether the 
// polygon is supposed to represent a line string ("0") or a linear ring ("1"). Each absolute node can be followed 
// by relative nodes: These relative nodes are always nodes that follow in correct order inside the polygon after 
// "their" absolute node. Each absolute node specifies the number of relative nodes which contain relative 
// coordinates in reference to their absolute node. So an absolute node provides the absolute reference for 
// relative nodes across a theoretical area of 2x2 squaredegree-area (which in practice frequently might rather 
// amount to 1x1 square degrees).
//
// So much of the compression works by just referencing lat/lon diffs to special "absolute nodes". Hence the 
// compression will especially work well for polygons with many nodes with a high node density.
//
// The parser has to convert these relative coordinates to absolute coordinates.
//
// Copyright 2012 Torsten Rahn <rahn@kde.org>
// Copyright 2012 Cezar Mocan <mocancezar@gmail.com>
//

#include "Pn2Runner.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

namespace Marble
{

// Polygon header flags, representing the type of polygon
enum polygonFlagType { LINESTRING = 0, LINEARRING = 1, OUTERBOUNDARY = 2, INNERBOUNDARY = 3, MULTIGEOMETRY = 4 };


Pn2Runner::Pn2Runner(QObject *parent) :
    ParsingRunner(parent)
{
}

Pn2Runner::~Pn2Runner()
{
}

bool Pn2Runner::errorCheckLat( qint16 lat ) 
{
    if ( lat >= -10800 && lat <= +10800 )
        return false;
    else
        return true;
}

bool Pn2Runner::errorCheckLon( qint16 lon )
{
    if ( lon >= -21600 && lon <= +21600 )
        return false;
    else
        return true;
}

bool Pn2Runner::importPolygon( QDataStream &stream, GeoDataLineString* linestring, quint32 nrAbsoluteNodes ) 
{
    qint16 lat, lon, nrRelativeNodes;
    qint8 relativeLat, relativeLon;
    bool error = false;


    for ( quint32 absoluteNode = 1; absoluteNode <= nrAbsoluteNodes; absoluteNode++ ) {
        stream >> lat >> lon >> nrRelativeNodes;

        error = error | errorCheckLat( lat ) | errorCheckLon( lon );

        qreal degLat = ( 1.0 * lat / 120.0 );
        qreal degLon = ( 1.0 * lon / 120.0 );

        GeoDataCoordinates *coord = new GeoDataCoordinates( degLon / 180 * M_PI, degLat / 180 * M_PI );
        linestring->append( *coord );

        for ( qint16 relativeNode = 1; relativeNode <= nrRelativeNodes; ++relativeNode ) {
            stream >> relativeLat >> relativeLon;

            qint16 currLat = relativeLat + lat; 
            qint16 currLon = relativeLon + lon;


            error = error | errorCheckLat( currLat ) | errorCheckLon( currLon );

            qreal currDegLat = ( 1.0 * currLat / 120.0 );
            qreal currDegLon = ( 1.0 * currLon / 120.0 );


            GeoDataCoordinates *currCoord = new GeoDataCoordinates( currDegLon / 180 * M_PI, currDegLat / 180 * M_PI );
            linestring->append( *currCoord );
        }
    }

    return error;
}

void Pn2Runner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFileInfo fileinfo( fileName );
    if( fileinfo.suffix().compare( "pn2", Qt::CaseInsensitive ) != 0 ) {
        emit parsingFinished( 0 );
        return;
    }

    QFile  file( fileName );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        emit parsingFinished( 0 );
        return;
    }

    file.open( QIODevice::ReadOnly );
    QDataStream stream( &file );  // read the data serialized from the file

    GeoDataDocument *document = new GeoDataDocument();
    document->setDocumentRole( role );

    quint8 fileHeaderVersion;
    quint32 fileHeaderPolygons;

    stream >> fileHeaderVersion >> fileHeaderPolygons;

    bool error = false;

    quint32 ID, nrAbsoluteNodes;
    quint8 flag, prevFlag = -1;

    GeoDataPolygon *polygon = new GeoDataPolygon;

    for ( quint32 currentPoly = 1; ( currentPoly <= fileHeaderPolygons ) && ( !error ) && ( !stream.atEnd() ); currentPoly++ ) {

        stream >> ID >> nrAbsoluteNodes >> flag;

        if ( flag != INNERBOUNDARY && ( prevFlag == INNERBOUNDARY || prevFlag == OUTERBOUNDARY ) ) {

            GeoDataPlacemark *placemark = new GeoDataPlacemark;
            placemark->setGeometry( polygon );
            document->append( placemark );
        }

        if ( flag == LINESTRING ) {
            GeoDataLineString *linestring = new GeoDataLineString;
            error = error | importPolygon( stream, linestring, nrAbsoluteNodes );

            GeoDataPlacemark *placemark = new GeoDataPlacemark;
            placemark->setGeometry( linestring );
            document->append( placemark );
        }

        if ( ( flag == LINEARRING ) || ( flag == OUTERBOUNDARY ) || ( flag == INNERBOUNDARY ) ) {
            GeoDataLinearRing* linearring = new GeoDataLinearRing;
            error = error | importPolygon( stream, linearring, nrAbsoluteNodes );

            if ( flag == LINEARRING ) {
                GeoDataPlacemark *placemark = new GeoDataPlacemark;
                placemark->setGeometry( linearring );
                document->append( placemark );
            }

            if ( flag == OUTERBOUNDARY ) {
                polygon = new GeoDataPolygon;
                polygon->setOuterBoundary( *linearring );
            }

            if ( flag == INNERBOUNDARY ) {
                polygon->appendInnerBoundary( *linearring );
            }
        }

        

        if ( flag == MULTIGEOMETRY ) {
            // not implemented yet, for now elements inside a multigeometry are separated as individual geometries
        }

        prevFlag = flag;
    }

    if ( prevFlag == INNERBOUNDARY || prevFlag == OUTERBOUNDARY ) {
        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        placemark->setGeometry( polygon );
        document->append( placemark );
    }

    if ( error ) {
        delete document;
        document = 0;
        emit parsingFinished( 0, "Errors occurred while parsing the .pn2 file!" );
        return;
    }
    document->setFileName( fileName );

    emit parsingFinished( document );
}

}

#include "Pn2Runner.moc"
