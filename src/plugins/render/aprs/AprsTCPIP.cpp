// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include "AprsTCPIP.h"

#include <QTcpSocket>

#include "MarbleDebug.h"

#include "AprsGatherer.h"

using namespace Marble;

AprsTCPIP::AprsTCPIP( const QString& hostName, int port )
    : m_hostName( hostName ),
      m_port( port ),
      m_numErrors( 0 )
{
}

AprsTCPIP::~AprsTCPIP()
{
}

QString
AprsTCPIP::sourceName() const
{
    return QStringLiteral( "TCP" );
}

bool
AprsTCPIP::canDoDirect() const
{
    return false;
}

QIODevice *
AprsTCPIP::openSocket() 
{
    m_numErrors = 0;

    QTcpSocket *socket = new QTcpSocket();
    mDebug() << "Opening TCPIP socket to " << 
        m_hostName.toLocal8Bit().data() << ":" << m_port;
    socket->connectToHost( m_hostName, m_port );
    socket->waitForReadyRead();

    {
        // read the initial server statement
        char buf[4096];
        socket->readLine( buf, sizeof( buf ) );
        mDebug() << "Aprs TCPIP server: " << buf;
    }

    QString towrite( "user MARBLE pass -1 vers aprs-cgi 1.0 filter r/38/-120/200\n" );
    socket->write( towrite.toLocal8Bit().data(), towrite.length() );

    mDebug() << "opened TCPIP socket";
    return socket;
}


void
AprsTCPIP::checkReadReturn( int length, QIODevice **socket,
                            AprsGatherer *gatherer ) 
{
    if ( length < 0 || ( length == 0 && m_numErrors > 5 ) ) {
        // hard error.  try reopening
        mDebug() << "**** restarting TCPIP socket";
        delete *socket;
        gatherer->sleepFor( 1 );
        *socket = openSocket();
        return;
    }
    if ( length == 0 ) {
        ++m_numErrors;
        mDebug() << "**** Odd: read zero bytes from TCPIP socket";
        return;
    }
    return;
}

