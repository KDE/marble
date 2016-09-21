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
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//


#include <QDebug>
#include <QVector>
#include <QFile>
#include <QDataStream>
#include <QApplication>
 
#include <MarbleWidget.h>
#include <MarbleModel.h>
#include <ParsingRunnerManager.h>
#include <GeoDataTreeModel.h>
#include <GeoDataFeature.h>
#include <GeoDataDocument.h>
#include <GeoDataStyle.h>
#include <GeoDataPolyStyle.h>
#include <GeoDataSchema.h>
#include <GeoDataSimpleField.h>
#include <GeoDataPlacemark.h>
#include <GeoDataLineString.h>
#include <GeoDataLinearRing.h>
#include <GeoDataPolygon.h>
#include <GeoDataPoint.h>
#include <GeoDataGeometry.h>
#include <GeoDataMultiGeometry.h>

using namespace Marble;

qreal epsilon   =   1.0;

// Polygon header flags, representing the type of polygon
enum polygonFlagType { LINESTRING = 0, LINEARRING = 1, OUTERBOUNDARY = 2, INNERBOUNDARY = 3, MULTIGEOMETRY = 4 };

qreal latDistance( const GeoDataCoordinates &A, const GeoDataCoordinates &B ) {
    qreal latA = A.latitude( GeoDataCoordinates::Degree );
    qreal latB = B.latitude( GeoDataCoordinates::Degree );
    return latB - latA;
}

qreal lonDistance( const GeoDataCoordinates &A, const GeoDataCoordinates &B ) {
    qreal lonA = A.longitude( GeoDataCoordinates::Degree );
    qreal lonB = B.longitude( GeoDataCoordinates::Degree );
    return lonB - lonA;
}

qreal nodeDistance( const GeoDataCoordinates &A, const GeoDataCoordinates &B ) {
    return qMax( qAbs( latDistance( A, B ) ), qAbs( lonDistance( A, B ) ) );
}


qint16 printFormat16( qreal X ) {
    return ( ( qint16 )( X * 120 ) );
}

qint8 printFormat8( qreal X ) {
    return ( ( qint8 )( X * 120 ) );
}

quint32 getParentNodes( QVector<GeoDataCoordinates>::Iterator begin, QVector<GeoDataCoordinates>::Iterator end )
{
    quint32 parentNodes = 0;

    QVector<GeoDataCoordinates>::Iterator it = begin;
    QVector<GeoDataCoordinates>::Iterator itAux = begin;

    for ( ; it != end && itAux != end; ++itAux ) {
        if ( ( nodeDistance( (*it), (*itAux) ) > epsilon ) || ( itAux == begin ) ) { // absolute nodes
            it = itAux;
            ++parentNodes;
        }
    }

    return parentNodes;
}

void printAllNodes( QVector<GeoDataCoordinates>::Iterator begin, QVector<GeoDataCoordinates>::Iterator end, QDataStream &stream ) 
{

    qint16 nrChildNodes; 

    QVector<GeoDataCoordinates>::Iterator it = begin;
    QVector<GeoDataCoordinates>::Iterator itAux = begin;

    for ( ; it != end && itAux != end; ++itAux ) {
        if ( ( nodeDistance( (*it), (*itAux) ) > epsilon ) || ( itAux == begin ) ) { // absolute nodes
            it = itAux;
            nrChildNodes = 0;
            QVector<GeoDataCoordinates>::Iterator itAux2 = it + 1;
            for ( ; itAux2 != end && nodeDistance( (*it), (*itAux2) ) <= epsilon; ++itAux2 )
                ++nrChildNodes;

            qint16 lat = printFormat16( it->latitude( GeoDataCoordinates::Degree ) );
            qint16 lon = printFormat16( it->longitude( GeoDataCoordinates::Degree ) );

            stream << lat << lon << nrChildNodes;
        }
        else { // relative nodes
            qint8 lat = printFormat8( latDistance( (*it), (*itAux) ) );
            qint8 lon = printFormat8( lonDistance( (*it), (*itAux) ) );
            stream << lat << lon;
        }
    }
}
 
int main(int argc, char** argv)
{
    QApplication app(argc,argv);


    qDebug() << " Syntax: shp2pn2 [-i shp-sourcefile -o pn2-targetfile]";

    QString inputFilename;
    int inputIndex = app.arguments().indexOf( "-i" );
    if ( inputIndex > 0 && inputIndex + 1 < argc )
        inputFilename = app.arguments().at( inputIndex + 1 );
    else {
	qWarning() << "Input file missing.";
	return 1;
    }

    QString outputFilename = "output.pn2";
    int outputIndex = app.arguments().indexOf("-o");
    if ( outputIndex > 0 && outputIndex + 1 < argc )
        outputFilename = app.arguments().at( outputIndex + 1 );
    

    MarbleModel *model = new MarbleModel;
    ParsingRunnerManager* manager = new ParsingRunnerManager( model->pluginManager() );

    GeoDataDocument* document = manager->openFile( inputFilename );
    if (!document) { 
      qWarning() << "Could not parse document (have you installed libshape?) !";
      return 1;
    }

    QFile file( outputFilename );
    file.open( QIODevice::WriteOnly );
    QDataStream stream( &file );

    quint8 fileHeaderVersion;
    quint32 fileHeaderPolygons;
    bool isMapColorField;

    fileHeaderVersion = 2;
    fileHeaderPolygons = 0; // This variable counts the number of polygons inside the document
    isMapColorField = false; // Whether the file contains mapcolor field or not.

    QVector<GeoDataFeature*>::Iterator i = document->begin();
    QVector<GeoDataFeature*>::Iterator const end = document->end();

    for (; i != end; ++i) {
        GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>( *i );

        // Types of placemarks
        GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon*>( placemark->geometry() );
        GeoDataLineString* linestring = dynamic_cast<GeoDataLineString*>( placemark->geometry() );
        GeoDataMultiGeometry* multigeom = dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() );

        if ( polygon ) {
            fileHeaderPolygons += 1 + polygon->innerBoundaries().size(); // outer boundary + number of inner boundaries of the polygon
        }

        if ( linestring ) {
            ++fileHeaderPolygons;
        }

        if ( multigeom ) {
            QVector<GeoDataGeometry*>::Iterator multi = multigeom->begin();
            QVector<GeoDataGeometry*>::Iterator multiEnd = multigeom->end();
            for ( ; multi != multiEnd; ++multi ) {
                /**
                 * Handle the no. of polygons in multigeom according to whether
                 * it contains GeoDataLineString or GeoDataPolygon
                 */
                GeoDataLineString *lineString = dynamic_cast<GeoDataLineString*>( *multi );
                GeoDataPolygon *poly  = dynamic_cast<GeoDataPolygon*>( *multi );
                if ( lineString ) {
                    ++fileHeaderPolygons;
                }
                if ( poly ) {
                    fileHeaderPolygons += 1 + poly->innerBoundaries().size();
                }
            }
        }
    }

    GeoDataSchema schema = document->schema( QString("default") );
    if (schema.simpleField("mapcolor13").name() == QLatin1String("mapcolor13")) {
        isMapColorField = true;
    }

    // Write in the beginnig whether the file contains mapcolor or not.
    stream << fileHeaderVersion << fileHeaderPolygons << isMapColorField;

    i = document->begin();

    quint32 placemarkCurrentID = 0;
    quint32 polyParentNodes;
    quint8 polyFlag;

    for ( ; i != end; ++i ) {
        GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>( *i );

        ++placemarkCurrentID;

        // Types of placemarks
        GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon*>( placemark->geometry() );
        GeoDataLineString* linestring = dynamic_cast<GeoDataLineString*>( placemark->geometry() );
        GeoDataMultiGeometry* multigeom = dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() );

        /**
         * For every placemark write a color index ( if isMapColorField is true )
         * and a placemarkID. In pn2 runner we will parse a color
         * index ( if it exists ) for every different placemarkID.
         * The general pattern is for writing values in pn2 file is:
         * flag -> placemarkID -> colorIndex -> polyParentNodes -> all nodes
         */

        if ( polygon ) {
            // Outer boundary
            QVector<GeoDataCoordinates>::Iterator jBegin = polygon->outerBoundary().begin();
            QVector<GeoDataCoordinates>::Iterator jEnd = polygon->outerBoundary().end();
            polyParentNodes = getParentNodes( jBegin, jEnd );
            polyFlag = OUTERBOUNDARY;

            stream << polyFlag << placemarkCurrentID;

            if ( isMapColorField ) {
                quint8 colorIndex = placemark->style()->polyStyle().colorIndex();
                stream << colorIndex;
            }

            stream << polyParentNodes;

            printAllNodes( jBegin, jEnd, stream );

            // Inner boundaries
            QVector<GeoDataLinearRing>::Iterator inner = polygon->innerBoundaries().begin();
            QVector<GeoDataLinearRing>::Iterator innerEnd = polygon->innerBoundaries().end();

            for ( ; inner != innerEnd; ++inner ) {
                GeoDataLinearRing linearring = static_cast<GeoDataLinearRing>( *inner );

                jBegin = linearring.begin();
                jEnd = linearring.end();
                polyParentNodes = getParentNodes( jBegin, jEnd );
                polyFlag = INNERBOUNDARY;

                stream << polyFlag << placemarkCurrentID << polyParentNodes;

                printAllNodes( jBegin, jEnd, stream );
            }

        }

        if ( linestring ) {
            QVector<GeoDataCoordinates>::Iterator jBegin = linestring->begin();
            QVector<GeoDataCoordinates>::Iterator jEnd = linestring->end();
            polyParentNodes = getParentNodes( jBegin, jEnd );
            if ( linestring->isClosed() )
                polyFlag = LINEARRING;
            else
                polyFlag = LINESTRING;

            stream << polyFlag << placemarkCurrentID;

            if ( isMapColorField ) {
                quint8 colorIndex = placemark->style()->polyStyle().colorIndex();
                stream << colorIndex;
            }

            stream << polyParentNodes;

            printAllNodes( jBegin, jEnd, stream );
        }

        if ( multigeom ) {
            QVector<GeoDataGeometry*>::Iterator multi = multigeom->begin();
            QVector<GeoDataGeometry*>::Iterator const multiEnd = multigeom->end();

            quint8 multiGeomSize = 0;

            for ( ; multi != multiEnd; ++multi ) {
                GeoDataLineString *lineString = dynamic_cast<GeoDataLineString*>( *multi );
                GeoDataPolygon *poly  = dynamic_cast<GeoDataPolygon*>( *multi );
                if ( lineString ) {
                    ++multiGeomSize;
                }
                if ( poly ) {
                    multiGeomSize += 1 + poly->innerBoundaries().size();
                }
            }

            multi = multigeom->begin();

            /**
             * While parsing pn2 whenever we encounter a MULTIGEOMETRY
             * flag we will proceed differently parsing @p multiGeomSize
             * GeoDataGeometry objects
             */

            polyFlag = MULTIGEOMETRY;

            stream << polyFlag << placemarkCurrentID;

            if ( isMapColorField ) {
                quint8 colorIndex = placemark->style()->polyStyle().colorIndex();
                stream << colorIndex;
            }

            stream << multiGeomSize;            

            for ( ; multi != multiEnd; ++multi ) {
                GeoDataLineString* currLineString = dynamic_cast<GeoDataLineString*>( *multi );
                GeoDataPolygon *currPolygon = dynamic_cast<GeoDataPolygon*>( *multi );

                if ( currLineString ) {
                    QVector<GeoDataCoordinates>::Iterator jBegin = currLineString->begin();
                    QVector<GeoDataCoordinates>::Iterator jEnd = currLineString->end();
                    polyParentNodes = getParentNodes( jBegin, jEnd );
                    if ( currLineString->isClosed() )
                        polyFlag = LINEARRING;
                    else
                        polyFlag = LINESTRING;

                    stream << polyFlag << placemarkCurrentID << polyParentNodes;

                    printAllNodes( jBegin, jEnd, stream );
                }

                else if ( currPolygon ) {
                    // Outer boundary
                    QVector<GeoDataCoordinates>::Iterator jBegin = currPolygon->outerBoundary().begin();
                    QVector<GeoDataCoordinates>::Iterator jEnd = currPolygon->outerBoundary().end();
                    polyParentNodes = getParentNodes( jBegin, jEnd );
                    polyFlag = OUTERBOUNDARY;

                    stream << polyFlag << placemarkCurrentID << polyParentNodes;

                    printAllNodes( jBegin, jEnd, stream );

                    // Inner boundaries
                    QVector<GeoDataLinearRing>::Iterator inner = currPolygon->innerBoundaries().begin();
                    QVector<GeoDataLinearRing>::Iterator innerEnd = currPolygon->innerBoundaries().end();

                    for ( ; inner != innerEnd; ++inner ) {
                        GeoDataLinearRing linearring = static_cast<GeoDataLinearRing>( *inner );

                        jBegin = linearring.begin();
                        jEnd = linearring.end();
                        polyParentNodes = getParentNodes( jBegin, jEnd );
                        polyFlag = INNERBOUNDARY;

                        stream << polyFlag << placemarkCurrentID << polyParentNodes;

                        printAllNodes( jBegin, jEnd, stream );
                    }
                }
            }
        }
    }
}
