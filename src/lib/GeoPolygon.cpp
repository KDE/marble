//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "GeoPolygon.h"

#include <stdlib.h>
#include <fcntl.h>
#include <cmath>

#include <QtCore/QFile>
#include <QtCore/QDataStream> 
#include <QtCore/QDebug>
#include <QtCore/QTime>
#ifdef Q_OS_UNIX
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/mman.h> /* mmap() is defined in this header */
#endif

const double ARCMINUTE = 10800; // distance of 180deg in arcminutes
const double INT2RAD = M_PI / 10800.0;

GeoPolygon::GeoPolygon()
{
    m_crossed = false;
    m_closed  = false;

    m_num = 0;
    m_x0  = 0;
    m_y0  = 0;
    m_x1  = 0;
    m_y1  = 0;
}

GeoPolygon::~GeoPolygon()
{
}


void GeoPolygon::setBoundary(int x0, int y0, int x1, int y1)
{
    m_x0 = x0;
    m_y0 = y0;
    m_x1 = x1;
    m_y1 = y1;

    m_boundary.clear();
    if ( getDateLine() ) {
        int xcenter = (int)( ( x0 + ( 2 * ARCMINUTE + x1) ) / 2 );

        if ( xcenter > ARCMINUTE ) 
            xcenter -= (int)( 2 * ARCMINUTE );
        if ( xcenter < -ARCMINUTE )
            xcenter += (int)( 2 * ARCMINUTE );

        m_boundary.append( GeoDataPoint( (double)(xcenter) * INT2RAD, (0.5 * (double)(y0 + y1)) * INT2RAD, 0.0, GeoDataPoint::Radian, 1 ) );
    }
    else
        m_boundary.append( GeoDataPoint( (0.5 * (double)(x0 + x1)) * INT2RAD, (0.5 * (double)(y0 + y1)) * INT2RAD, 0.0, GeoDataPoint::Radian, 1 ) );

    m_boundary.append(GeoDataPoint( (double)(x0) * INT2RAD, (double)(y0) * INT2RAD, 0.0, GeoDataPoint::Radian, 1 ));
    m_boundary.append(GeoDataPoint( (double)(x1) * INT2RAD, (double)(y1) * INT2RAD, 0.0, GeoDataPoint::Radian, 1 ));
    m_boundary.append(GeoDataPoint( (double)(x1) * INT2RAD, (double)(y0) * INT2RAD, 0.0, GeoDataPoint::Radian, 1 ));
    m_boundary.append(GeoDataPoint( (double)(x0) * INT2RAD, (double)(y1) * INT2RAD, 0.0, GeoDataPoint::Radian, 1 ));

}


// ================================================================
//                               class PntMap


PntMap::PntMap()
{
}

PntMap::~PntMap()
{
    qDeleteAll( begin(), end() );
}


void PntMap::load(const QString &filename)
{
//    qDebug("PntMap::load trying to load: " + filename.toLocal8Bit());
    QTime *timer = new QTime();
    timer->restart();

#ifdef Q_OS_UNIX
    // MMAP Start
    int          fd;
    unsigned char* src; 
    struct stat  statbuf;

    if ( (fd = open (filename.toLatin1(), O_RDONLY) ) < 0)
        qDebug() << "cannot open" << filename << " for reading";

    if ( fstat (fd,&statbuf) < 0 )
        qDebug() << "fstat error";

    int  filelength = statbuf.st_size;
	
    if ((src = (unsigned char*) mmap (0, filelength, PROT_READ, MAP_SHARED, fd, 0)) == (unsigned char*) (caddr_t) -1)
        qDebug() << "mmap error for input";
		
    short  header;
    short  iLat;
    short  iLon;
    int    count = 0;

    //const int halfFileLength = filelength / 2;

    for (int i=0; i < filelength; i+=6){
        header = src[i] | (src[i+1] << 8);
        iLat = src[i+2] | (src[i+3] << 8);
        iLon = src[i+4] | (src[i+5] << 8);   

        // Transforming Range of Coordinates to iLat [0,ARCMINUTE] ,
        // iLon [0,2 * ARCMINUTE]
						
//        iLat = -iLat;

        //
        // 90 00N =   -ARCMINUTE / 2
        // 90 00S =   ARCMINUTE / 2
        // 180 00W =  -ARCMINUTE
        // 180 00E =   ARCMINUTE
        //
        if ( header > 5 ) {
			
            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());

            GeoPolygon  *polyline = new GeoPolygon();
            append( polyline );

            polyline->setNum( header );

            // Find out whether the Polyline is a river or a closed polygon
            if ( ( header >= 7000 && header < 8000 )
                 || ( header >= 9000 && header < 20000 ) )
                polyline->setClosed( false );
            else 
                polyline->setClosed( true );

            polyline->append( GeoDataPoint( (double)(iLon) * INT2RAD, (double)(iLat) * INT2RAD, 0.0, GeoDataPoint::Radian, 5 ) );
        }
        else {
            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());
            last()->append( GeoDataPoint( (double)(iLon) * INT2RAD, (double)(iLat) * INT2RAD, 0.0, GeoDataPoint::Radian, (int)(header) ) ); 
        }
        ++count;
    }
    munmap(src,filelength);
    close(fd);

    // qDebug(QString("Opened %1 with %2 Polylines and %3 Points").arg(filename).arg(this->count()).arg(count).toLatin1());
    // MMAP End
#else
#  ifdef Q_OS_WIN

    // qDebug("Loading PntMap ...");
    int    count = 0;
    QFile  file( filename );

    file.open( QIODevice::ReadOnly );
    QDataStream stream( &file );  // read the data serialized from the file
    stream.setByteOrder( QDataStream::LittleEndian );

    short  header;
    short  iLat;
    short  iLon;

    // Iterator that points to current PolyLine in PntMap
    //	QList<GeoPolygon*>::iterator it = begin();
    //	int count = 0;

    while( !stream.atEnd() ){	
        stream >> header >> iLat >> iLon;		
        // Transforming Range of Coordinates to iLat [0,ARCMINUTE] , iLon [0,2 * ARCMINUTE] 
						
//        iLat = -iLat;
        if ( header > 5 ) {
			
            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());
            GeoPolygon  *polyline = new GeoPolygon();
            append( polyline );

            polyline->setNum(header);

            // Find out whether the Polyline is a river or a closed polygon
            if ( ( header >= 7000 && header < 8000 )
                 || ( header >= 9000 && header < 20000 ) )
                polyline->setClosed( false );
            else 
                polyline->setClosed( true );

            polyline->append( GeoDataPoint( (double)(iLon) * INT2RAD, (double)(iLat) * INT2RAD, 0.0, GeoDataPoint::Radian, 5 ) );
        }
        else {
            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());
            last()->append( GeoDataPoint( (double)(iLon) * INT2RAD, (double)(iLat) * INT2RAD, 0.0, GeoDataPoint::Radian, (int)(header) ) );
        }
        ++count;
    }
	
    file.close();
    // qDebug(QString("Opened %1 with %2 Polylines and %3 Points").arg(filename).arg(this->count()).arg(count).toLatin1());

#  else
#  warning Your OS is not supported!
#  endif
#endif


    // To optimize performance we compute the boundaries of the
    // polygons.  To detect inside/outside we need to detect the
    // dateline first We probably won't need this for spherical
    // projection but for flat projection
    //
    // FIXME: Break this out into its own function.
		
    double  lon     = 0.0;
    double  lastLon = 0.0;
    double  lat     = 0.0;
    int lastSign    = 0;

    GeoPolygon::PtrVector::Iterator       itPolyLine;
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = end();
    GeoDataPoint::Vector::ConstIterator   itPoint;

    // Detect for all Polylines, whether they are crossing the international date line 
    for ( itPolyLine = begin(); itPolyLine != itEndPolyLine; ++itPolyLine ) {

        GeoDataPoint::Vector::Iterator  itEndPoint = (*itPolyLine)->end();

        for ( itPoint = (*itPolyLine)->begin(); 
              itPoint != itEndPoint;
              ++itPoint )
        {
            (*itPoint).geoCoordinates(lon, lat);

            int currentSign = ( lon > 0.0 ) ? 1 : -1 ;
            if ( itPoint == (*itPolyLine)->begin() )
                lastSign = currentSign;

            if ( lastSign != currentSign && fabs(lastLon) + fabs(lon) > M_PI )
            {
                    (*itPolyLine)->setDateLine(true);
                    itPoint = itEndPoint - 1;
            }
            lastLon = lon;
        }	
    }

    // Now we calculate the boundaries
    double x = 0;
    double y = 0;
	
    for ( itPolyLine = begin(); itPolyLine != itEndPolyLine; ++itPolyLine ) {
		
        double  lonLeft = ARCMINUTE;
        double  lonRight = -ARCMINUTE;
        double  latTop = ARCMINUTE / 2.0;
        double  latBottom = -ARCMINUTE / 2.0;		
        GeoDataPoint::Vector::ConstIterator  itEndPoint = (*itPolyLine)->end();

        if ( (*itPolyLine)->getDateLine() ) { 
					
            for ( itPoint = (*itPolyLine)->begin();
                  itPoint != itEndPoint;
                  ++itPoint )
            {
                (*itPoint).geoCoordinates( lon, lat );
                x = (int)( ARCMINUTE * lon / M_PI );

                if ( x < lonLeft  && x > -ARCMINUTE / 2 ) lonLeft = x;
                if ( x > lonRight  && x < -ARCMINUTE / 2 ) lonRight = x;
				
                y = (int)( ARCMINUTE * lat / M_PI );

                if ( y < latTop ) latTop = y;
                if ( y > latBottom ) latBottom = y;
            }

            (*itPolyLine)->setBoundary( (int)(lonLeft), (int)(latTop),
                                        (int)(lonRight), (int)(latBottom) );
            // (*itPolyLine)->displayBoundary();
        }		
        else {			
            for ( itPoint = (*itPolyLine)->begin();
                  itPoint != itEndPoint;
                  ++itPoint )
            {
                double  lon;
                double  lat;
                (*itPoint).geoCoordinates( lon, lat );
                x = (int)( ARCMINUTE * lon / M_PI );

                if (x < lonLeft) lonLeft = x;
                if (x > lonRight) lonRight = x;
				
                y = (int)( ARCMINUTE * lat / M_PI );

                if (y < latTop) latTop = y;
                if (y > latBottom) latBottom = y;
            }
            (*itPolyLine)->setBoundary( (int)(lonLeft), (int)(latTop),
                                        (int)(lonRight), (int)(latBottom) );
            // (*itPolyLine)->displayBoundary();
        }
        // qDebug() << "Test" << (int)(lonLeft) << (int)(latTop) << (int)(lonRight) << (int)(latBottom);
        // (*itPolyLine)->setBoundary((int)(lonLeft),(int)(latTop),(int)(lonRight),(int)(latBottom));
        // (*itPolyLine)->displayBoundary();
    }
//    qDebug() << "Elapsed: " << timer->elapsed();
    delete timer;
}
