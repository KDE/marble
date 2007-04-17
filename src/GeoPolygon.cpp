//
// C++ Implementation: pntmap
//
// Description: 
//
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <stdlib.h>
#include <fcntl.h>
#include <cmath>
#include <unistd.h>

#include <QtCore/QFile>
#include <QtCore/QDataStream> 
#include <QtCore/QDebug>
#include <QtCore/QTime>
#ifdef Q_OS_UNIX
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/mman.h> /* mmap() is defined in this header */
#endif

#include "GeoPolygon.h"


const float ARCMINUTE = 10800;

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

        m_boundary.append( GeoPoint( 1, xcenter, (y0 + y1)/2 ) );
    }
    else
        m_boundary.append( GeoPoint( 1, (x0 + x1)/2, (y0 + y1)/2 ) );

    m_boundary.append(GeoPoint( 1, x0, y0));
    m_boundary.append(GeoPoint( 1, x1, y1));
    m_boundary.append(GeoPoint( 1, x1, y0));
    m_boundary.append(GeoPoint( 1, x0, y1));
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
    QTime *timer = new QTime();
    timer->restart();

#ifdef Q_OS_UNIX
    // MMAP Start
    int          fd;
    short       *src; 
    struct stat  statbuf;

    if ( (fd = open (filename.toLatin1(), O_RDONLY) ) < 0)
        qDebug() << "can't open" << filename << " for reading";

    if ( fstat (fd,&statbuf) < 0 )
        qDebug() << "fstat error";

    int  filelength = statbuf.st_size;
	
    if ( ( src = (short*) mmap (0, 2*filelength, PROT_READ, MAP_SHARED, fd, 0) )
         == (short*) (caddr_t) -1 )
        qDebug() << "mmap error for input";
		
    short  header;
    short  lat;
    short  lng;
    int    count = 0;

    const int halfFileLength = filelength / 2;

    for ( int i = 0; i < halfFileLength; i += 3 ) {
        header = src[i];
        lat = src[i+1];
        lng = src[i+2];	

        // Transforming Range of Coordinates to lat [0,ARCMINUTE] ,
        // lng [0,2 * ARCMINUTE]
						
        lat = -lat;

        //
        // 90 00N =   -ARCMINUTE / 2
        // 90 00S =   ARCMINUTE / 2
        // 180 00W =  -ARCMINUTE
        // 180 00E =   ARCMINUTE
        //
        if ( header > 5 ) {
			
            // qDebug(QString("header: %1 lat: %2 lng: %3").arg(header).arg(lat).arg(lng).toLatin1());

            GeoPolygon  *polyline = new GeoPolygon();
            append( polyline );

            polyline->setNum( header );

            // Find out whether the Polyline is a river or a closed polygon
            if ( ( header >= 7000 && header < 8000 )
                 || ( header >= 9000 && header < 20000 ) )
                polyline->setClosed( false );
            else 
                polyline->setClosed( true );

            polyline->append( GeoPoint( 5, (int)(lng), (int)(lat) ) );
        }
        else {
            // qDebug(QString("header: %1 lat: %2 lng: %3").arg(header).arg(lat).arg(lng).toLatin1());
            last()->append( GeoPoint( (int)(header), (int)(lng), (int)(lat) ) ); 
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
    short  lat;
    short  lng;

    // Iterator that points to current PolyLine in PntMap
    //	QList<GeoPolygon*>::iterator it = begin();
    //	int count = 0;

    while( !stream.atEnd() ){	
        stream >> header >> lat >> lng;		
        // Transforming Range of Coordinates to lat [0,ARCMINUTE] , lng [0,2 * ARCMINUTE] 
						
        lat = -lat;
        if ( header > 5 ) {
			
            // qDebug(QString("header: %1 lat: %2 lng: %3").arg(header).arg(lat).arg(lng).toLatin1());
            GeoPolygon  *polyline = new GeoPolygon();
            append( polyline );

            polyline->setNum(header);

            // Find out whether the Polyline is a river or a closed polygon
            if ( ( header >= 7000 && header < 8000 )
                 || ( header >= 9000 && header < 20000 ) )
                polyline->setClosed( false );
            else 
                polyline->setClosed( true );

            polyline->append( GeoPoint( 5, (int)(lng), (int)(lat) ) );
        }
        else {
            // qDebug(QString("header: %1 lat: %2 lng: %3").arg(header).arg(lat).arg(lng).toLatin1());
            last()->append( GeoPoint( (int)(header), (int)(lng), (int)(lat) ) );
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
		
    float  x     = 0.0;
    float  lastx = 0.0;

    GeoPolygon::PtrVector::Iterator       itPolyLine;
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = end();
    GeoPoint::Vector::ConstIterator       itPoint;

    for ( itPolyLine = begin(); itPolyLine != itEndPolyLine; ++itPolyLine ) {

        GeoPoint::Vector::Iterator  itEndPoint = (*itPolyLine)->end();

        for ( itPoint = (*itPolyLine)->begin(); 
              itPoint != itEndPoint;
              ++itPoint )
        {
            float  lon;
            float  lat;
            (*itPoint).geoCoordinates(lon, lat);
            x = (int)( ARCMINUTE * lon / M_PI );

            if (lastx != 0) {
                if ( x/lastx < 0.0
                     && ( abs((int)x)+abs((int)lastx) ) > ARCMINUTE )
                {
                    (*itPolyLine)->setDateLine(true);
                    // qDebug() << "DateLine: " << lastx << x;
                    itPoint = itEndPoint - 1;
                }
            }
            lastx = x;
        }	
    }

#if 0
    for ( itPolyLine = begin(); itPolyLine != itEndPolyLine; ++itPolyLine ) {


        const QVector<GeoPoint*>::const_iterator itEndPoint = (*itPolyLine)->end();

        for ( itPoint = (*itPolyLine)->begin(); itPoint != itEndPoint; ++itPoint ){
            (*itPoint)->setMul(6.0f);
        }	
    }
#endif

    // Now we calculate the boundaries
    float y = 0;
	
    for ( itPolyLine = begin(); itPolyLine != itEndPolyLine; ++itPolyLine ) {
		
        float  x0 = ARCMINUTE;
        float  x1 = -ARCMINUTE;
        float  y0 = ARCMINUTE / 2.0;
        float  y1 = -ARCMINUTE / 2.0;		
        GeoPoint::Vector::ConstIterator  itEndPoint = (*itPolyLine)->end();

        if ( (*itPolyLine)->getDateLine() ) { 
					
            for ( itPoint = (*itPolyLine)->begin();
                  itPoint != itEndPoint;
                  ++itPoint )
            {
                float  lon;
                float  lat;

                (*itPoint).geoCoordinates( lon, lat );
                x = (int)( ARCMINUTE * lon / M_PI );

                if ( x < x0  && x > -ARCMINUTE / 2 ) x0 = x;
                if ( x > x1  && x < -ARCMINUTE / 2 ) x1 = x;
				
                y = (int)( ARCMINUTE * lat / M_PI );

                if ( y < y0 ) y0 = y;
                if ( y > y1 ) y1 = y;
            }

            (*itPolyLine)->setBoundary( (int)(x0), (int)(y0),
                                        (int)(x1), (int)(y1) );
            // (*itPolyLine)->displayBoundary();
        }		
        else {			
            for ( itPoint = (*itPolyLine)->begin();
                  itPoint != itEndPoint;
                  ++itPoint )
            {
                float  lon;
                float  lat;
                (*itPoint).geoCoordinates( lon, lat );
                x = (int)( ARCMINUTE * lon / M_PI );

                if (x < x0) x0 = x;
                if (x > x1) x1 = x;
				
                y = (int)( ARCMINUTE * lat / M_PI );

                if (y < y0) y0 = y;
                if (y > y1) y1 = y;
            }
            (*itPolyLine)->setBoundary( (int)(x0), (int)(y0),
                                        (int)(x1), (int)(y1) );
            // (*itPolyLine)->displayBoundary();
        }
        // qDebug() << "Test" << (int)(x0) << (int)(y0) << (int)(x1) << (int)(y1);
        // (*itPolyLine)->setBoundary((int)(x0),(int)(y0),(int)(x1),(int)(y1));
        // (*itPolyLine)->displayBoundary();
    }

    delete timer;
}
