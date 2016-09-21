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

#include "Pn2Runner.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoDataMultiGeometry.h"
#include "MarbleDebug.h"

#include <QFile>
#include <QFileInfo>

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

        GeoDataCoordinates coord( degLon / 180 * M_PI, degLat / 180 * M_PI );
        linestring->append( coord );

        for ( qint16 relativeNode = 1; relativeNode <= nrRelativeNodes; ++relativeNode ) {
            stream >> relativeLat >> relativeLon;

            qint16 currLat = relativeLat + lat; 
            qint16 currLon = relativeLon + lon;


            error = error | errorCheckLat( currLat ) | errorCheckLon( currLon );

            qreal currDegLat = ( 1.0 * currLat / 120.0 );
            qreal currDegLon = ( 1.0 * currLon / 120.0 );


            GeoDataCoordinates currCoord( currDegLon / 180 * M_PI, currDegLat / 180 * M_PI );
            linestring->append( currCoord );
        }
    }

    *linestring = linestring->optimized();

    return error;
}

GeoDataDocument *Pn2Runner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    QFileInfo fileinfo( fileName );
    if (fileinfo.suffix().compare(QLatin1String("pn2"), Qt::CaseInsensitive) != 0) {
        error = QStringLiteral("File %1 does not have a pn2 suffix").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    QFile  file( fileName );
    if ( !file.exists() ) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    file.open( QIODevice::ReadOnly );
    m_stream.setDevice( &file );  // read the data serialized from the file

    m_stream >> m_fileHeaderVersion >> m_fileHeaderPolygons >> m_isMapColorField;

    switch( m_fileHeaderVersion ) {
        case 1: return parseForVersion1( fileName, role );
                break;
        case 2: return parseForVersion2( fileName, role );
                break;
        default: qDebug() << "File can't be parsed. We don't have parser for file header version:" << m_fileHeaderVersion;
                break;
    }

    return nullptr;
}

GeoDataDocument* Pn2Runner::parseForVersion1(const QString& fileName, DocumentRole role)
{
    GeoDataDocument *document = new GeoDataDocument();
    document->setDocumentRole( role );

    bool error = false;

    quint32 ID, nrAbsoluteNodes;
    quint8 flag, prevFlag = -1;

    GeoDataStyle::Ptr style;
    GeoDataPolygon *polygon = new GeoDataPolygon;

    for ( quint32 currentPoly = 1; ( currentPoly <= m_fileHeaderPolygons ) && ( !error ) && ( !m_stream.atEnd() ); currentPoly++ ) {

        m_stream >> ID >> nrAbsoluteNodes >> flag;

        if ( flag != INNERBOUNDARY && ( prevFlag == INNERBOUNDARY || prevFlag == OUTERBOUNDARY ) ) {

            GeoDataPlacemark *placemark = new GeoDataPlacemark;
            placemark->setGeometry( polygon );
            if ( m_isMapColorField ) {
                if ( style ) {
                    placemark->setStyle( style );
                }
            }
            document->append( placemark );
        }

        if ( flag == LINESTRING ) {
            GeoDataLineString *linestring = new GeoDataLineString;
            error = error | importPolygon( m_stream, linestring, nrAbsoluteNodes );

            GeoDataPlacemark *placemark = new GeoDataPlacemark;
            placemark->setGeometry( linestring );
            document->append( placemark );
        }

        if ( ( flag == LINEARRING ) || ( flag == OUTERBOUNDARY ) || ( flag == INNERBOUNDARY ) ) {
            if ( flag == OUTERBOUNDARY && m_isMapColorField ) {
                quint8 colorIndex;
                m_stream >> colorIndex;
                style = GeoDataStyle::Ptr(new GeoDataStyle);
                GeoDataPolyStyle polyStyle;
                polyStyle.setColorIndex( colorIndex );
                style->setPolyStyle( polyStyle );
            }

            GeoDataLinearRing* linearring = new GeoDataLinearRing;
            error = error | importPolygon( m_stream, linearring, nrAbsoluteNodes );

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
        if ( m_isMapColorField ) {
            if ( style ) {
                placemark->setStyle( style );
            }
        }
        placemark->setGeometry( polygon );
        document->append( placemark );
    }

    if ( error ) {
        delete document;
        document = 0;
        return nullptr;
    }
    document->setFileName( fileName );
    return document;
}

GeoDataDocument* Pn2Runner::parseForVersion2( const QString &fileName, DocumentRole role )
{
    GeoDataDocument *document = new GeoDataDocument();
    document->setDocumentRole( role );

    bool error = false;

    quint32 nrAbsoluteNodes;
    quint32 placemarkCurrentID = 1;
    quint32 placemarkPrevID = 0;
    quint8 flag, prevFlag = -1;

    GeoDataPolygon *polygon = new GeoDataPolygon;
    GeoDataStyle::Ptr style;
    GeoDataPlacemark *placemark =0; // new GeoDataPlacemark;

    quint32 currentPoly;
    for ( currentPoly = 1; ( currentPoly <= m_fileHeaderPolygons ) && ( !error ) && ( !m_stream.atEnd() ); currentPoly++ ) {
        m_stream >> flag >> placemarkCurrentID;

        if ( flag == MULTIGEOMETRY && ( prevFlag == INNERBOUNDARY || prevFlag == OUTERBOUNDARY ) ) {
            if ( placemark ) {
                placemark->setGeometry( polygon );
            }
        }

        if ( flag != MULTIGEOMETRY && flag != INNERBOUNDARY && ( prevFlag == INNERBOUNDARY || prevFlag == OUTERBOUNDARY ) ) {
            if ( placemark ) {
                placemark->setGeometry( polygon );
            }
        }
        /**
         * If the parsed placemark id @p placemarkCurrentID is different
         * from the id of previous placemark @p placemarkPrevID, it means
         * we have encountered a new placemark. So, prepare a style @p style
         * if file has color indices
         */
        if ( placemarkCurrentID != placemarkPrevID ) {
            placemark = new GeoDataPlacemark;

            // Handle the color index
            if( m_isMapColorField ) {
                quint8 colorIndex;
                m_stream >> colorIndex;
                style = GeoDataStyle::Ptr(new GeoDataStyle);
                GeoDataPolyStyle polyStyle;
                polyStyle.setColorIndex( colorIndex );
                polyStyle.setFill( true );
                style->setPolyStyle( polyStyle );
                placemark->setStyle( style );
            }

            document->append( placemark );
        }

        placemarkPrevID = placemarkCurrentID;

        if ( flag != MULTIGEOMETRY ) {
            m_stream >> nrAbsoluteNodes;

            if ( flag == LINESTRING ) {
                GeoDataLineString *linestring = new GeoDataLineString;
                error = error | importPolygon( m_stream, linestring, nrAbsoluteNodes );
                if ( placemark ) {
                    placemark->setGeometry( linestring );
                }
            }

            if ( ( flag == LINEARRING ) || ( flag == OUTERBOUNDARY ) || ( flag == INNERBOUNDARY ) ) {
                GeoDataLinearRing* linearring = new GeoDataLinearRing;
                error = error || importPolygon( m_stream, linearring, nrAbsoluteNodes );

                if ( flag == LINEARRING ) {
                    if ( placemark ) {
                        placemark->setGeometry( linearring );
                    }
                } else {
                    if ( flag == OUTERBOUNDARY ) {
                        polygon = new GeoDataPolygon;
                        polygon->setOuterBoundary( *linearring );
                    }

                    if ( flag == INNERBOUNDARY ) {
                        polygon->appendInnerBoundary( *linearring );
                    }

                    delete linearring;
                }
            }
            prevFlag = flag;
        }

        else {
            quint32 placemarkCurrentIDInMulti;
            quint8 flagInMulti;
            quint8 prevFlagInMulti = -1;
            quint8 multiSize = 0;

            m_stream >> multiSize;

            GeoDataMultiGeometry *multigeom = new GeoDataMultiGeometry;

            /**
             * Read @p multiSize GeoDataGeometry objects
             */
            for ( int iter = 0; iter < multiSize; ++iter ) {
                m_stream >> flagInMulti >> placemarkCurrentIDInMulti >> nrAbsoluteNodes;
                if ( flagInMulti != INNERBOUNDARY && ( prevFlagInMulti == INNERBOUNDARY || prevFlagInMulti == OUTERBOUNDARY ) ) {
                    multigeom->append( polygon );
                }

                if ( flagInMulti == LINESTRING ) {
                    GeoDataLineString *linestring = new GeoDataLineString;
                    error = error || importPolygon( m_stream, linestring, nrAbsoluteNodes );
                    multigeom->append( linestring );
                }

                if ( ( flagInMulti == LINEARRING ) || ( flagInMulti == OUTERBOUNDARY ) || ( flagInMulti == INNERBOUNDARY ) ) {
                    GeoDataLinearRing* linearring = new GeoDataLinearRing;
                    error = error | importPolygon( m_stream, linearring, nrAbsoluteNodes );

                    if ( flagInMulti == LINEARRING ) {
                        multigeom->append( linearring );
                    } else {
                        if ( flagInMulti == OUTERBOUNDARY ) {
                            polygon = new GeoDataPolygon;
                            polygon->setOuterBoundary( *linearring );
                        }

                        if ( flagInMulti == INNERBOUNDARY ) {
                            polygon->appendInnerBoundary( *linearring );
                        }

                        delete linearring;
                    }
                }
                prevFlagInMulti = flagInMulti;
            }

            if ( prevFlagInMulti == INNERBOUNDARY || prevFlagInMulti == OUTERBOUNDARY ) {
                multigeom->append( polygon );
            }
            if ( placemark ) {
                placemark->setGeometry( multigeom );
            }
            prevFlag = MULTIGEOMETRY;
        }
    }

    if ( (prevFlag == INNERBOUNDARY || prevFlag == OUTERBOUNDARY) && prevFlag != MULTIGEOMETRY ) {
        placemark->setGeometry( polygon );
    }

    if ( error ) {
        delete document;
        document = 0;
        return nullptr;
    }
    document->setFileName( fileName );
    return document;
}

}


#include "moc_Pn2Runner.cpp"
