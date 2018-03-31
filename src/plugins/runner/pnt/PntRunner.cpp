//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "PntRunner.h"

#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"
#include "MarbleGlobal.h"

#include <qmath.h>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>

namespace Marble
{

// distance of 180deg in arcminutes
const qreal INT2RAD = M_PI / 10800.0;

PntRunner::PntRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

PntRunner::~PntRunner()
{
}

GeoDataDocument *PntRunner::parseFile(const QString &fileName, DocumentRole role, QString &errorString)
{
    QFileInfo fileinfo( fileName );
    if (fileinfo.suffix().compare(QLatin1String("pnt"), Qt::CaseInsensitive) != 0) {
        errorString = QStringLiteral("File %1 does not have a pnt suffix").arg(fileName);
        mDebug() << errorString;
        return nullptr;
    }

    QFile  file( fileName );
    if ( !file.exists() ) {
        errorString = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << errorString;
        return nullptr;
    }

    file.open( QIODevice::ReadOnly );
    QDataStream stream( &file );  // read the data serialized from the file
    stream.setByteOrder( QDataStream::LittleEndian );

    GeoDataDocument *document = new GeoDataDocument();
    document->setDocumentRole( role );
    GeoDataPlacemark  *placemark = nullptr;

    int count = 0;
    bool error = false;
    while( !stream.atEnd() || error ){
        short  header = -1;
        short  iLat = -5400 - 1;
        short  iLon = -10800 - 1;

        stream >> header >> iLat >> iLon;

        // make sure iLat is within valid range
        if ( !( -5400 <= iLat && iLat <= 5400 ) ) {
            mDebug() << Q_FUNC_INFO << "invalid iLat =" << iLat << "(" << ( iLat * INT2RAD ) * RAD2DEG << ") in dataset" << count << "of file" << fileName;
            error = true;
        }

        // make sure iLon is within valid range
        if ( !( -10800 <= iLon && iLon <= 10800 ) ) {
            mDebug() << Q_FUNC_INFO << "invalid iLon =" << iLon << "(" << ( iLon * INT2RAD ) * RAD2DEG << ") in dataset" << count << "of file" << fileName;
            error = true;
        }

        if (header >= 1000 && !document->isEmpty()) {
            GeoDataLineString *const polyline = static_cast<GeoDataLineString*>( placemark->geometry() );
            if ( polyline->size() == 1 ) {
                mDebug() << Q_FUNC_INFO << fileName << "contains single-point polygon at" << count << ". Aborting.";
                error = true;
                break;
            }
        }

        if ( header < 1 ) {
            /* invalid header */
            mDebug() << Q_FUNC_INFO << "invalid header:" << header << "in" << fileName << "at" << count;
            error = true;
            break;
        }
        else if ( header <= 5 ) {
            /* header represents level of detail */
            /* nothing to do */
        }
        else if ( header < 1000 ) {
            /* invalid header */
            mDebug() << Q_FUNC_INFO << "invalid header:" << header << "in" << fileName << "at" << count;
            error = true;
            break;
        }
        else if ( header < 2000 ) {
            /* header represents start of coastline */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLinearRing );
        }
        else if ( header < 4000 ) {
            /* header represents start of country border */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLineString );
        }
        else if ( header < 5000 ) {
            /* header represents start of internal political border */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLineString );
        }
        else if ( header < 6000 ) {
            /* header represents start of island */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLinearRing );
        }
        else if ( header < 7000 ) {
            /* header represents start of lake */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLinearRing );
        }
        else if ( header < 8000 ) {
            /* header represents start of river */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLineString );
        }
        else if ( header < 9000 ) {
            /* custom header represents start of glaciers, lakes or islands */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLinearRing );
        }
        else if ( header < 10000 ) {
            /* custom header represents start of political borders */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLineString );
        }
        else if ( header < 14000 ) {
            /* invalid header */
            mDebug() << Q_FUNC_INFO << "invalid header:" << header << "in" << fileName << "at" << count;
            error = true;
            break;
        }
        else if ( header < 15000 ) {
            /* custom header represents start of political borders */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLineString );
        }
        else if ( header < 19000 ) {
            /* invalid header */
            mDebug() << Q_FUNC_INFO << "invalid header:" << header << "in" << fileName << "at" << count;
            error = true;
            break;
        }
        else if ( header < 20000 ) {
            /* custom header represents start of dateline */
            placemark = new GeoDataPlacemark;
            document->append( placemark );
            placemark->setGeometry( new GeoDataLineString );
        }
        else {
            /* invalid header */
            mDebug() << Q_FUNC_INFO << "invalid header:" << header << "in" << fileName << "at" << count;
            error = true;
            break;
        }

        GeoDataLineString *polyline = static_cast<GeoDataLineString*>( placemark->geometry() );

        // Transforming Range of Coordinates to iLat [0,ARCMINUTE] , iLon [0,2 * ARCMINUTE]
        polyline->append( GeoDataCoordinates( (qreal)(iLon) * INT2RAD, (qreal)(iLat) * INT2RAD,
                                              0.0, GeoDataCoordinates::Radian,
                                              5 - qMin( 5, (int)header  ) ) ); // if 1 <= header <= 5, header contains level of detail
                                                                           // else pick most sparse level of detail, which equals 0

        ++count;
    }

    file.close();
    if (document->isEmpty() || error) {
        delete document;
        document = nullptr;
        return nullptr;
    }
    document->setFileName( fileName );
    return document;
}

}

#include "moc_PntRunner.cpp"
