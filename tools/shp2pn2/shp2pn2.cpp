#include <QtCore/QDebug>
#include <QtCore/QVector>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDataStream>
#include <QtGui/QApplication>
#include <QtGui/QTreeView>
 
#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/MarbleRunnerManager.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/GeoDataFeature.h>
#include <marble/GeoDataDocument.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataLineString.h>
#include <marble/GeoDataLinearRing.h>
#include <marble/GeoDataPolygon.h>
#include <marble/GeoDataPoint.h>
#include <marble/GeoDataGeometry.h>
 
using namespace Marble;

qreal epsilon   =   1.0;
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
    return ( qAbs( latDistance( A, B ) ) + qAbs( lonDistance( A, B ) ) );
}


qint16 printFormat16( const qreal &X ) {
    return ( ( qint16 )( X * 120 ) );
}

qint8 printFormat8( const qreal &X ) {
    return ( ( qint8 )( X * 120 ) );
}


quint32 getParentNodes( QVector<GeoDataCoordinates>::Iterator begin, QVector<GeoDataCoordinates>::Iterator end )
{
    quint32 parentNodes = 0;

    QVector<GeoDataCoordinates>::Iterator it = begin;
    QVector<GeoDataCoordinates>::Iterator itAux = begin;

    for ( ; it != end && itAux != end; ++itAux ) {
        if ( ( nodeDistance( (*it), (*itAux) ) > epsilon ) || ( itAux == begin ) ) { // absolute node
            it = itAux;
            parentNodes++;
        }
    }

    return parentNodes;
}

void printAllNodes( QVector<GeoDataCoordinates>::Iterator begin, QVector<GeoDataCoordinates>::Iterator end, QDataStream &stream ) 
{

    quint16 nrChildNodes; 

    QVector<GeoDataCoordinates>::Iterator it = begin;
    QVector<GeoDataCoordinates>::Iterator itAux = begin;

    for ( ; it != end && itAux != end; ++itAux ) {
        if ( ( nodeDistance( (*it), (*itAux) ) > epsilon ) || ( itAux == begin ) ) { // absolute node
            it = itAux;
            nrChildNodes = 0;
            QVector<GeoDataCoordinates>::Iterator itAux2 = it + 1;
            for ( ; itAux2 != end && nodeDistance( (*it), (*itAux2) ) <= epsilon; ++itAux2 )
                nrChildNodes++;

            qint16 lat = printFormat16( it->latitude( GeoDataCoordinates::Degree ) );
            qint16 lon = printFormat16( it->longitude( GeoDataCoordinates::Degree ) );

            stream << nrChildNodes << lat << lon;
        }
        else {
            qint8 lat = printFormat8( latDistance( (*it), (*itAux) ) );
            qint8 lon = printFormat8( lonDistance( (*it), (*itAux) ) );
            stream << lat << lon;
        }
    }
}
 
int main(int argc, char** argv)
{
    QApplication app(argc,argv);


    qDebug( " Syntax: pnt2svg [-i shp-sourcefile -o pn2-targetfile]" );

    QString inputFilename;
    int inputIndex = app.arguments().indexOf( "-i" );
    if ( inputIndex > 0 && inputIndex + 1 < argc )
        inputFilename = app.arguments().at( inputIndex + 1 );

    QString outputFilename = "output.pn2";
    int outputIndex = app.arguments().indexOf("-o");
    if ( outputIndex > 0 && outputIndex + 1 < argc )
        outputFilename = app.arguments().at( outputIndex + 1 );
    
 
    MarbleModel *model = new MarbleModel;
    MarbleRunnerManager* manager = new MarbleRunnerManager( model->pluginManager() );
 
    GeoDataDocument* document = manager->openFile( inputFilename );

    QFile file( outputFilename );
    file.open( QIODevice::WriteOnly );
    QDataStream stream( &file );

    quint8 fileHeaderVersion;
    quint32 fileHeaderPolygons;

    fileHeaderVersion = 1;
    fileHeaderPolygons = 0;

    QVector<GeoDataFeature*>::Iterator i = document->begin();
    QVector<GeoDataFeature*>::Iterator const end = document->end();

    for (; i != end; ++i) {
        GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>( *i );

        // Types of placemarks
        GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon*>( placemark->geometry() );
        GeoDataLineString* linestring = dynamic_cast<GeoDataLineString*>( placemark->geometry() );
        GeoDataMultiGeometry* multigeom = dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() );

        if ( polygon ) {
            fileHeaderPolygons += 1 + polygon->innerBoundaries().size(); // outer boundary + number of inner boundaries
        }

        if ( linestring ) {
            fileHeaderPolygons++;
        }

        if ( multigeom ) {
            fileHeaderPolygons += multigeom->size();
        }
    }

    stream << fileHeaderVersion << fileHeaderPolygons;

    i = document->begin();

    quint32 polyCurrentID = 0;
    quint32 polyParentNodes;
    quint8 polyFlag; 
    // 0 - linestring
    // 1 - linearring
    // 2 - outer polygon
    // 3 - inner polygon
    // 4 - multigeometry (not used yet)

    for ( ; i != end; ++i ) {
        GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>( *i );

        // Types of placemarks
        GeoDataPolygon* polygon = dynamic_cast<GeoDataPolygon*>( placemark->geometry() );
        GeoDataLineString* linestring = dynamic_cast<GeoDataLineString*>( placemark->geometry() );
        GeoDataMultiGeometry* multigeom = dynamic_cast<GeoDataMultiGeometry*>( placemark->geometry() );

        if ( polygon ) {

            // Outer boundary
            polyCurrentID++;
            QVector<GeoDataCoordinates>::Iterator jBegin = polygon->outerBoundary().begin();
            QVector<GeoDataCoordinates>::Iterator jEnd = polygon->outerBoundary().end();
            polyParentNodes = getParentNodes( jBegin, jEnd );
            polyFlag = OUTERBOUNDARY; // outer boundary

            stream << polyCurrentID << polyParentNodes << polyFlag;

            printAllNodes( jBegin, jEnd, stream );

            // Inner boundaries
            QVector<GeoDataLinearRing>::Iterator inner = polygon->innerBoundaries().begin();
            QVector<GeoDataLinearRing>::Iterator innerEnd = polygon->innerBoundaries().end();

            for ( ; inner != innerEnd; ++inner ) {
                GeoDataLinearRing linearring = static_cast<GeoDataLinearRing>( *inner );

                polyCurrentID++;
                jBegin = linearring.begin();
                jEnd = linearring.end();
                polyParentNodes = getParentNodes( jBegin, jEnd );
                polyFlag = INNERBOUNDARY; // inner boundary

                stream << polyCurrentID << polyParentNodes << polyFlag;

                printAllNodes( jBegin, jEnd, stream );
               
            }

        }

        if ( linestring ) {
            polyCurrentID++;
            QVector<GeoDataCoordinates>::Iterator jBegin = linestring->begin();
            QVector<GeoDataCoordinates>::Iterator jEnd = linestring->end();
            polyParentNodes = getParentNodes( jBegin, jEnd );
            if ( linestring->isClosed() )
                polyFlag = LINEARRING; // linearring
            else
                polyFlag = LINESTRING; // linestring

            stream << polyCurrentID << polyParentNodes << polyFlag;

            printAllNodes( jBegin, jEnd, stream );
        }

        if ( multigeom ) {

            QVector<GeoDataGeometry*>::Iterator multi = multigeom->begin();
            QVector<GeoDataGeometry*>::Iterator multiEnd = multigeom->end();
    
            for ( ; multi != multiEnd; ++multi ) {
                GeoDataLineString* currLineString = dynamic_cast<GeoDataLineString*>( *multi );

                polyCurrentID++;
                QVector<GeoDataCoordinates>::Iterator jBegin = currLineString->begin();
                QVector<GeoDataCoordinates>::Iterator jEnd = currLineString->end();
                polyParentNodes = getParentNodes( jBegin, jEnd );
                if ( currLineString->isClosed() )
                    polyFlag = LINEARRING; // linearring
                else
                    polyFlag = LINESTRING; // linestring

                stream << polyCurrentID << polyParentNodes << polyFlag;

                printAllNodes( jBegin, jEnd, stream );
            }
            
        }
    }

}
