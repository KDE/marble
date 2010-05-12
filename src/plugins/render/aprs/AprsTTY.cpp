//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include "AprsTTY.h"
#include "qextserialport.h"

#include "MarbleDebug.h"

#include "AprsGatherer.h"

using namespace Marble;

AprsTTY::AprsTTY( QString ttyName )
    : m_ttyName( ttyName ),
      m_numErrors( 0 )
{
}

AprsTTY::~AprsTTY()
{
}

QString
AprsTTY::sourceName() const
{
    return QString( "TTY" );
}

bool
AprsTTY::canDoDirect() const
{
    return true;
}

QIODevice *
AprsTTY::openSocket() 
{
    QextSerialPort *m_port =
        new QextSerialPort( m_ttyName, QextSerialPort::Polling );
    m_port->setBaudRate( BAUD9600 );
    m_port->setParity( PAR_NONE );
    m_port->setDataBits( DATA_8 );
    m_port->setStopBits( STOP_1 );
    m_port->setTimeout( 60000 ); // ms
    m_port->open( QIODevice::ReadWrite );
    mDebug() << "opened TTY socket";
    if ( m_port->isOpen() ) {
        mDebug() << "connected to " << m_ttyName.toLocal8Bit().data();
    } else {
        delete m_port;
        m_port = 0;
        mDebug() << "**** failed to open terminal " << m_ttyName.toLocal8Bit().data() << " ****";
    }
    return m_port;
}

void
AprsTTY::checkReadReturn( int length, QIODevice **socket,
                          AprsGatherer *gatherer ) 
{
    if ( length < 0 || ( length == 0 && m_numErrors > 5 ) ) {
        // hard error.  try reopening
        mDebug() << "**** restarting TTY socket";
        delete *socket;
        gatherer->sleepFor( 1 );
        *socket = openSocket();
        return;
    }
    if ( length == 0 ) {
        ++m_numErrors;
        mDebug() << "**** Odd: read zero bytes from TTY socket";
        return;
    }
    return;
}

