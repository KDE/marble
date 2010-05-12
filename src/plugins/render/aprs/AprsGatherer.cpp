//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include <QtGui/QPixmap>

#include "AprsGatherer.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

using namespace Marble;

AprsGatherer::AprsGatherer( AprsSource *from,
                            QMap<QString, AprsObject *> *objects,
                            QMutex *mutex,
                            QString *filter )
    : m_source( from ),
      m_socket( 0 ),
      m_filter( filter ),
      m_running( true ),
      m_dumpOutput( false ),
      m_seenFrom( GeoAprsCoordinates::FromNowhere ),
      m_sourceName( ),
      m_mutex( mutex ),
      m_objects( objects )
{
    m_sourceName = from->sourceName();
    initMicETables();
}

AprsGatherer::AprsGatherer( QIODevice *from,
                            QMap<QString, AprsObject *> *objects,
                            QMutex *mutex,
                            QString *filter ) 
    : m_source( 0 ),
      m_socket( from ),
      m_filter( filter ),
      m_running( true ),
      m_dumpOutput( false ),
      m_seenFrom( GeoAprsCoordinates::FromNowhere ),
      m_sourceName( "unknown" ),
      m_mutex( mutex ),
      m_objects( objects )
{
    initMicETables();
}

void 
AprsGatherer::run() 
{
    char buf[4096];
    qint64 linelength;
    // one particular APRS packet sender can add data after the : ( sigh )
    QRegExp matcher( "^([0-9A-Z]+-*[0-9A-Z]*)>([^:]*):([!=@\\/])([0-9][0-9][0-9][0-9][0-9][0-9]|)([hz\\/]|)([0-9][0-9])([0-9][0-9]\\.[0-9][0-9])([NS])(.)([0-9][0-9][0-9])([0-9][0-9]\\.[0-9][0-9])([EW])(.)" );

    // mic-e formatted
    // 1: src
    // 2: dst
    // 3: routes
    // 4: longitude x 3
    // 5: speed and course x3
    // 6: symbol and symbol ID
    // 7: status text
    QRegExp mic_e_matcher( "^([0-9A-Z]+-*[0-9A-Z]*)>([^,:]*),*([^:]*):['`](...)(...)(..)(.*)" );

    // If a source can directly receive a signal (as opposed to
    // through a relay like the internet) will return true.  This
    // prevents accidentially coloring signals heard over some sources
    // as heard directly where it's never possible (such as over the
    // internet).
    bool canDoDirect = m_source->canDoDirect();
    
    while( m_running ) {

        if ( m_socket && !m_socket->isOpen() ) {
            // connection closed; attempt to reopen
            mDebug() << "aprs: socket closed; attempting to reopen";
            delete m_socket;
            m_socket = 0;
        }
        
            
        if ( !m_socket && m_source )
            m_socket = m_source->openSocket();

        if ( !m_socket ) {
            mDebug() << "aprs: failed to open socket from "
                     << m_sourceName.toLocal8Bit().data();
            sleep( 5 );
            continue;
        }
        
        // wait for data to read in
        if ( m_socket->bytesAvailable() <= 0 )
            // wait no longer than 1s
            if ( ! m_socket->waitForReadyRead( 1000 ) )
                continue; // continue to loop again on "not ready"

        // Read the line to parse
        linelength = m_socket->readLine( buf, sizeof( buf ) );

        // if we got 0 or less bytes this is probably an odd case; ask
        // the source what to do.
        m_source->checkReadReturn( linelength, &m_socket, this );
        
        if ( linelength <= 0 ) {
            // don't go into an infinite untimed loop of failed sockets
            sleep( 2 );
            continue;
        }

        if ( m_socket && m_filter != NULL ) {
            QMutexLocker locker( m_mutex );
            if ( m_filter->length() > 0 ) {
                m_socket->write( m_filter->toLocal8Bit().data(),
                                 m_filter->length() );
            }
        }

        // Parse the results
        QString line( buf );

        // Dump it out if we wanted it dumped
        if ( m_dumpOutput )
            mDebug() << "aprs: " << m_sourceName.toLocal8Bit().data()
                     << ": " << line;

        if ( matcher.indexIn( line ) != -1 ) {
            QString callSign  = matcher.cap( 1 );
            qreal latitude = matcher.cap( 6 ).toFloat() +
                ( matcher.cap( 7 ).toFloat()/60 );
            if ( matcher.cap( 8 ) == "S" )
                latitude = - latitude;

            qreal longitude = matcher.cap( 10 ).toFloat() +
                ( matcher.cap( 11 ).toFloat()/60 );
            if ( matcher.cap( 12 ) == "W" )
                longitude = - longitude;

            addObject( callSign, latitude, longitude, canDoDirect,
                       QString( matcher.cap( 2 ) ),
                       QChar( matcher.cap( 9 )[0] ),
                       QChar( matcher.cap( 13 )[0] ) );
        }
        else if ( mic_e_matcher.indexIn( line ) != -1 ) {
            // MIC-E formatted compressed packet
            QString myCall  = mic_e_matcher.cap( 1 ); 
            QString dstCall = mic_e_matcher.cap( 2 );

            qreal latitude =
                // hours
                m_dstCallDigits[dstCall[0]] * 10 +
                m_dstCallDigits[dstCall[1]] +

                // minutes
                ( qreal( m_dstCallDigits[dstCall[2]] * 10 +
                         m_dstCallDigits[dstCall[3]] ) +
                  qreal( m_dstCallDigits[dstCall[4]] ) / 10.0 +
                  qreal( m_dstCallDigits[dstCall[5]] ) / 100 ) / 60.0;

            if ( m_dstCallSouthEast[dstCall[4]] )
                latitude = - latitude;

            qreal longitude =
                calculateLongitude( QString ( mic_e_matcher.cap( 4 ) ),
                                    m_dstCallLongitudeOffset[dstCall[5]],
                                    m_dstCallSouthEast[dstCall[6]] );

//           mDebug() << "  MIC-E: " << line.toLocal8Bit().data();
//           mDebug() << "    lat: " << latitude;
//           mDebug() << "    lon: " << longitude;

            addObject( myCall, latitude, longitude, canDoDirect,
                       QString( mic_e_matcher.cap( 3 ) ),
                       QChar( mic_e_matcher.cap( 6 )[1] ),
                       QChar( mic_e_matcher.cap( 6 )[0] ) );
        }
        else {
            mDebug() << "aprs: UNPARSED: " << line;
        }

        // If the filter should be changed, send it out the socket
        if ( m_filter != NULL ) {
            QMutexLocker locker( m_mutex );
            if ( m_filter->length() > 0 ) {
                m_socket->write( m_filter->toLocal8Bit().data(),
                                 m_filter->length() );
            }
        }
    }
}

void
AprsGatherer::shutDown()
{
    m_running = false;
}

void
AprsGatherer::addObject( const QString &callSign,
                         qreal latitude, qreal longitude, bool canDoDirect,
                         const QString &routePath,
                         const QChar &symbolTable,
                         const QChar &symbolCode )
{
    AprsObject *foundObject = 0;
    QMutexLocker locker( m_mutex );
        
    int this_seenFrom = m_seenFrom;
    if ( canDoDirect ) {
        if ( !routePath.contains( QChar( '*' ) ) ) {
            this_seenFrom |= GeoAprsCoordinates::Directly;
        }
    }

    if ( m_objects->contains( callSign ) ) {
        // we already have one for this callSign; just add the new
        // history item.
        ( *m_objects )[callSign]->setLocation( longitude, latitude,
                                               this_seenFrom );
        ( *m_objects )[callSign]->setSeenFrom( this_seenFrom );

        // mDebug() << "  is old";
    }
    else {
        foundObject = new AprsObject( longitude, latitude, callSign,
                                      this_seenFrom );
        QString s = m_pixmaps[QPair<QChar, QChar>( '/','*' )];
        foundObject->setPixmapId( m_pixmaps[QPair<QChar, QChar>( symbolTable,symbolCode )] );
        ( *m_objects )[callSign] = foundObject;
        mDebug() << "aprs:  new: " << callSign.toLocal8Bit().data();
        // foundObject->setTarget( "earth" );
    }
    //emit repaintNeeded( QRegion() );
}


#include "AprsGatherer.moc"

void AprsGatherer::initMicETables()
{
#include "AprsGatherer_mic_e.h"
}

qreal AprsGatherer::calculateLongitude( const QString &threeBytes, int offset,
                                         bool isEast )
{
    // otherwise known as "fun with funky encoding"
    qreal hours = threeBytes[0].toAscii() - 28 + offset;
    if ( 180 <= hours && hours <= 189 )
        hours -= 80;
    if ( 190 <= hours && hours <= 199 )
        hours -= 190;

    hours +=
        ( qreal( (threeBytes[1].toAscii() - 28 ) % 60 ) + 
          ( qreal( threeBytes[2].toAscii() - 28 ) ) / 100 ) / 60.0;

    if ( ! isEast )
        hours = -hours;
    return hours;
}

void
AprsGatherer::setDumpOutput( bool to )
{
    m_dumpOutput = to;
}

bool
AprsGatherer::dumpOutput()
{
    return m_dumpOutput;
}

void
AprsGatherer::setSeenFrom( GeoAprsCoordinates::SeenFrom to )
{
    m_seenFrom = to;
}

GeoAprsCoordinates::SeenFrom
AprsGatherer::seenFrom()
{
    return m_seenFrom;
}

// gets around parent QThread protected sleep as our objects need a sleep
void
AprsGatherer::sleepFor(int seconds)
{
    sleep(seconds);
}
