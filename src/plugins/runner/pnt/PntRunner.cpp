//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "PntRunner.h"

#include "GeoDataDocument.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

namespace Marble
{

// distance of 180deg in arcminutes
const qreal INT2RAD = M_PI / 10800.0;

PntRunner::PntRunner(QObject *parent) :
    MarbleAbstractRunner(parent)
{
}

PntRunner::~PntRunner()
{
}

GeoDataFeature::GeoDataVisualCategory PntRunner::category() const
{
    return GeoDataFeature::Folder;
}

void PntRunner::parseFile( const QString &fileName, DocumentRole role = UnknownDocument )
{
    QFileInfo fileinfo( fileName );
    if( fileinfo.suffix().compare( "pnt", Qt::CaseInsensitive ) != 0 ) {
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
    stream.setByteOrder( QDataStream::LittleEndian );

    GeoDataDocument *document = new GeoDataDocument();
    document->setDocumentRole( role );

    short  header;
    short  iLat;
    short  iLon;

    GeoDataPlacemark  *placemark = 0;
    placemark = new GeoDataPlacemark;
    document->append( placemark );
    GeoDataMultiGeometry *geom = new GeoDataMultiGeometry;
    placemark->setGeometry( geom );

    while( !stream.atEnd() ){
        stream >> header >> iLat >> iLon;
        // Transforming Range of Coordinates to iLat [0,ARCMINUTE] , iLon [0,2 * ARCMINUTE]

        if ( header > 5 ) {

            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());

            // Find out whether the Polyline is a river or a closed polygon
            if ( ( header >= 7000 && header < 8000 )
                || ( header >= 9000 && header < 20000 ) ) {
                GeoDataLineString *polyline = new GeoDataLineString;
                geom->append( polyline );
            }
            else {
                GeoDataLinearRing *polyline = new GeoDataLinearRing;
                geom->append( polyline );
            }
        }
        GeoDataLineString *polyline = static_cast<GeoDataLineString*>(geom->child(geom->size()-1));
        polyline->append( GeoDataCoordinates( (qreal)(iLon) * INT2RAD, (qreal)(iLat) * INT2RAD,
                                                  0.0, GeoDataCoordinates::Radian, 5 ) );
    }

    file.close();
    if ( geom->size() ) {
        emit parsingFinished( document );
    } else {
        emit parsingFinished( 0 );
    }
}

}

#include "PntRunner.moc"
