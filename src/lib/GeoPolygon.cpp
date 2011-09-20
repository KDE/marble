//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#include "GeoPolygon.h"

#include <cstdlib>
#include <fcntl.h>
#include <cmath>
using std::fabs;

#include <QtCore/QFile>
#include <QtCore/QDataStream> 
#include <QtCore/QTime>
#ifdef Q_OS_UNIX
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/mman.h> /* mmap() is defined in this header */
#endif

#include "MarbleDebug.h"
#include "Quaternion.h"

using namespace Marble;

const qreal ARCMINUTE = 10800; // distance of 180deg in arcminutes
const qreal INT2RAD = M_PI / 10800.0;

GeoPolygon::GeoPolygon()
    : m_dateLineCrossing( false ),
      m_closed( false ),
      m_lonLeft( 0.0 ),
      m_latTop( 0.0 ),
      m_lonRight( 0.0 ),
      m_latBottom( 0.0 ),
      m_index( 0 )
{
}

GeoPolygon::~GeoPolygon()
{
//    qDeleteAll( begin(), end() );
    qDeleteAll( m_boundary );
}

void GeoPolygon::setBoundary( qreal lonLeft, qreal latTop, qreal lonRight, qreal latBottom)
{
    m_lonLeft   = lonLeft;
    m_latTop    = latTop;
    m_lonRight  = lonRight;
    m_latBottom = latBottom;

    m_boundary.clear();
    if ( getDateLine() == GeoPolygon::Even ) {
        qreal xcenter = ( lonLeft + ( 2.0 * M_PI + lonRight) ) / 2.0;

        if ( xcenter > M_PI ) 
            xcenter -=  2.0 * M_PI;
        if ( xcenter < -M_PI )
            xcenter +=  2.0 * M_PI;

        m_boundary.append( new GeoDataCoordinates( xcenter, 0.5 * (latTop + latBottom), 0.0,
                                                   GeoDataCoordinates::Radian, 1 ) );
    }
    else
        m_boundary.append( new GeoDataCoordinates( 0.5 * (lonLeft + lonRight),
                                                   0.5 * (latTop + latBottom), 0.0,
                                                   GeoDataCoordinates::Radian, 1 ) );

    m_boundary.append( new GeoDataCoordinates( lonLeft,  latTop,    0.0, GeoDataCoordinates::Radian, 1 ));
    m_boundary.append( new GeoDataCoordinates( lonRight, latBottom, 0.0, GeoDataCoordinates::Radian, 1 ));
    m_boundary.append( new GeoDataCoordinates( lonRight, latTop,    0.0, GeoDataCoordinates::Radian, 1 ));
    m_boundary.append( new GeoDataCoordinates( lonLeft,  latBottom, 0.0, GeoDataCoordinates::Radian, 1 ));

}

void GeoPolygon::displayBoundary()
{
    Quaternion  q;
    qreal      lon;
    qreal      lat;
    m_boundary.at(0)->geoCoordinates(lon, lat, GeoDataCoordinates::Degree);
    mDebug() << "Boundary:" << lon << ", " << lat;
    m_boundary.at(1)->geoCoordinates(lon, lat, GeoDataCoordinates::Degree);
    mDebug() << "Boundary:" << lon << ", " << lat;
    m_boundary.at(2)->geoCoordinates(lon, lat, GeoDataCoordinates::Degree);
    mDebug() << "Boundary:" << lon << ", " << lat;
    m_boundary.at(3)->geoCoordinates(lon, lat, GeoDataCoordinates::Degree);
    mDebug() << "Boundary:" << lon << ", " << lat;
    m_boundary.at(4)->geoCoordinates(lon, lat, GeoDataCoordinates::Degree);
    mDebug() << "Boundary:" << lon << ", " << lat;

//     mDebug() << "Points#: " << size() << " File: " << m_sourceFileName
//              << " dateline " << getDateLine() << " Index: " << getIndex();
}

// ================================================================
//                               class PntMap


PntMap::PntMap()
    : m_isInitialized( false ),
      m_loader( 0 )
{
}

bool PntMap::isInitialized() const
{
    return m_isInitialized;
}

PntMap::~PntMap()
{   
    if ( m_loader ) {
        m_loader->wait();
    }
    qDeleteAll( begin(), end() );
}

void PntMap::load(const QString &filename)
{
    m_loader = new PntMapLoader( this, filename );

    connect ( m_loader, SIGNAL( pntMapLoaded( bool ) ), SLOT( setInitialized( bool ) ) );
    m_loader->start();
}

void PntMap::setInitialized( bool isInitialized )
{
    if ( m_loader->isFinished() ) {
        delete m_loader;
        m_loader = 0;
    }

    m_isInitialized = isInitialized;
    emit initialized();
}

PntMapLoader::PntMapLoader( PntMap* parent, const QString& filename )
    : m_parent( parent ),
      m_filename( filename )
{
}

void PntMapLoader::run()
{
//    qDebug("PntMap::load trying to load: " + m_filename.toLocal8Bit());
    QTime timer;
    timer.restart();

#ifdef Q_OS_UNIX
    // MMAP Start
    int          fd;
    unsigned char* src; 
    struct stat  statbuf;

    if ( (fd = open (m_filename.toLatin1(), O_RDONLY) ) < 0)  // krazy:exclude=syscalls
        mDebug() << "cannot open" << m_filename << " for reading";

    if ( fstat (fd,&statbuf) < 0 ) // krazy:exclude=syscalls
        mDebug() << "fstat error";

    int  filelength = statbuf.st_size;
	
    if ((src = (unsigned char*) mmap (0, filelength, PROT_READ, MAP_SHARED, fd, 0))
        == (unsigned char*) (caddr_t) -1)
        mDebug() << "mmap error for input";
		
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
						
        //
        // 90 00N =   -ARCMINUTE / 2
        // 90 00S =   ARCMINUTE / 2
        // 180 00W =  -ARCMINUTE
        // 180 00E =   ARCMINUTE
        //
        if ( header > 5 ) {
			
            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());

            GeoPolygon  *polyline = new GeoPolygon();
            m_parent->append( polyline );

//            polyline->m_sourceFileName=filename;
            polyline->setIndex( header );

            // Find out whether the Polyline is a river or a closed polygon
            if ( ( header >= 7000 && header < 8000 )
                 || ( header >= 9000 && header < 20000 ) )
                polyline->setClosed( false );
            else 
                polyline->setClosed( true );

            polyline->append( GeoDataCoordinates( (qreal)(iLon) * INT2RAD, (qreal)(iLat) * INT2RAD,
                                                  0.0, GeoDataCoordinates::Radian, 5 ) );
        }
        else {
            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());
            m_parent->last()->append( GeoDataCoordinates( (qreal)(iLon) * INT2RAD,
                                                          (qreal)(iLat) * INT2RAD, 0.0,
                                                          GeoDataCoordinates::Radian,
                                                          (int)(header) ) ); 
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
    QFile  file( m_filename );

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
						
        if ( header > 5 ) {
			
            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());
            GeoPolygon  *polyline = new GeoPolygon();
            m_parent->append( polyline );

            polyline->setIndex(header);

            // Find out whether the Polyline is a river or a closed polygon
            if ( ( header >= 7000 && header < 8000 )
                 || ( header >= 9000 && header < 20000 ) )
                polyline->setClosed( false );
            else 
                polyline->setClosed( true );

            polyline->append( GeoDataCoordinates( (qreal)(iLon) * INT2RAD, (qreal)(iLat) * INT2RAD,
                                                  0.0, GeoDataCoordinates::Radian, 5 ) );
        }
        else {
            // qDebug(QString("header: %1 iLat: %2 iLon: %3").arg(header).arg(iLat).arg(iLon).toLatin1());
            m_parent->last()->append( GeoDataCoordinates( (qreal)(iLon) * INT2RAD,
                                                          (qreal)(iLat) * INT2RAD, 0.0,
                                                          GeoDataCoordinates::Radian,
                                                          (int)(header) ) );
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
		
    qreal  lon     = 0.0;
    qreal  lastLon = 0.0;
    qreal  lat     = 0.0;

    GeoPolygon::PtrVector::ConstIterator       itPolyLine;
    GeoPolygon::PtrVector::ConstIterator  itEndPolyLine = m_parent->constEnd();
    GeoDataCoordinates::Vector::ConstIterator   itPoint;

    // Now we calculate the boundaries
	
    for ( itPolyLine = m_parent->constBegin(); itPolyLine != itEndPolyLine; ++itPolyLine ) {
		
        qreal  lonLeft       =  +M_PI;
        qreal  lonRight      =  -M_PI;
        qreal  otherLonLeft  =  +M_PI;
        qreal  otherLonRight =  -M_PI;
        qreal  latTop        =  -M_PI / 2.0;
        qreal  latBottom     =  +M_PI / 2.0;		

        bool isCrossingDateLine = false;
        bool isOriginalSide = true;
        int  lastSign     = 0;

        GeoDataCoordinates::Vector::ConstIterator  itEndPoint = (*itPolyLine)->constEnd();
					
        for ( itPoint = (*itPolyLine)->constBegin();
                itPoint != itEndPoint;
                ++itPoint )
        {
            itPoint->geoCoordinates( lon, lat );

            int currentSign = ( lon > 0.0 ) ? 1 : -1 ;

            if( itPoint == (*itPolyLine)->constBegin() ) {
                lastSign = currentSign;
                lastLon  = lon;
            }

            if ( lastSign != currentSign && fabs(lastLon) + fabs(lon) > M_PI ) {
                isOriginalSide = !isOriginalSide;
                isCrossingDateLine = true;
            }

            if ( isOriginalSide ) {
                if ( lon < lonLeft  ) lonLeft = lon;
                if ( lon > lonRight ) lonRight = lon;
            } else {
                if ( lon < otherLonLeft  ) otherLonLeft = lon;
                if ( lon > otherLonRight ) otherLonRight = lon;
            }

            if ( lat > latTop )    latTop = lat;
            if ( lat < latBottom ) latBottom = lat;

            lastSign = currentSign;
            lastLon  = lon;
        }

        if ( !isOriginalSide ) {
            (*itPolyLine)->setDateLine( GeoPolygon::Odd );
//            mDebug() << "Odd  >> File: " << (*itPolyLine)->m_sourceFileName;
            (*itPolyLine)->setBoundary( -M_PI, latTop, M_PI, -M_PI / 2.0 );
//            mDebug() << " lonLeft: " << lonLeft << " lonRight: " << lonRight << " otherLonLeft: " << otherLonLeft << " otherlonRight: " << otherLonRight;
        }

        if ( isOriginalSide && isCrossingDateLine ) {
            (*itPolyLine)->setDateLine( GeoPolygon::Even );
//            mDebug() << "Even >> File: " << (*itPolyLine)->m_sourceFileName << " Size: " << (*itPolyLine)->size();

//            mDebug() << " lonLeft: " << lonLeft << " lonRight: " << lonRight << " otherLonLeft: " << otherLonLeft << " otherlonRight: " << otherLonRight;

            qreal leftLonRight, rightLonLeft;

            if ( fabs( M_PI * lonRight/fabs(lonRight) - lonRight ) >  
                 fabs( M_PI * otherLonRight/fabs(otherLonRight) - otherLonRight ) ) {
                rightLonLeft = otherLonLeft; 
                leftLonRight = lonRight; 
            } else {
                rightLonLeft = lonLeft;
                leftLonRight = otherLonRight;
            }

            (*itPolyLine)->setBoundary( rightLonLeft, latTop, leftLonRight, latBottom );

//            mDebug() << "Crosses: lonLeft: " << rightLonLeft << " is right from: lonRight: " << leftLonRight;

        }
        if ( !isCrossingDateLine ) {
            (*itPolyLine)->setDateLine( GeoPolygon::None );
            (*itPolyLine)->setBoundary( lonLeft, latTop, lonRight, latBottom );
        }
    }

    mDebug() << Q_FUNC_INFO << "Loaded" << m_filename << "in" << timer.elapsed() << "ms";

    emit pntMapLoaded( true );
}

#include "GeoPolygon.moc"
