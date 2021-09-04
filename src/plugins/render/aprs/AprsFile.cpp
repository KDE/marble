// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include "AprsFile.h"

#include <QFile>

#include "MarbleDebug.h"

#include "AprsGatherer.h"

using namespace Marble;

AprsFile::AprsFile( const QString &fileName )
    : m_fileName( fileName ),
      m_errorCount( 0 )
{
}

AprsFile::~AprsFile()
{
}

QString
AprsFile::sourceName() const
{
    return QString( "File" );
}

bool
AprsFile::canDoDirect() const
{
    return true;
}

QIODevice *
AprsFile::openSocket() 
{
    QFile *file = new QFile( m_fileName );
    
    mDebug() << "opening File socket";
    if ( !file->open( QFile::ReadOnly ) ) {
        mDebug() << "opening File failed";
        delete file;
        return nullptr;
    }
    mDebug() << "Opened " << m_fileName.toLocal8Bit().data();
    return file;
}

void
AprsFile::checkReadReturn( int length, QIODevice **socket,
                           AprsGatherer *gatherer ) 
{
    Q_UNUSED( socket );
    if ( length < 0 || ( length == 0 && m_errorCount > 5 ) ) {
        gatherer->sleepFor( 1 ); // just wait for more
        return;
    }
    if ( length == 0 ) {
        ++m_errorCount;
        mDebug() << "**** Odd: read zero bytes from File socket";
    }
    return;
}
