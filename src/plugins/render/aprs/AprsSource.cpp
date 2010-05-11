//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include "AprsSource.h"

using namespace Marble;

AprsSource::AprsSource( QIODevice *insocket )
    : m_socket( insocket )
{
}

AprsSource::~AprsSource()
{
}

QIODevice *
AprsSource::socket() 
{
    return m_socket;
}

void
AprsSource::setSocket( QIODevice *setto ) 
{
    m_socket = setto;
}

//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include "AprsSource.h"

using namespace Marble;

AprsSource::AprsSource( QIODevice *insocket )
    : m_socket( insocket )
{
}

AprsSource::~AprsSource()
{
}

QIODevice *
AprsSource::socket() 
{
    return m_socket;
}

void
AprsSource::setSocket( QIODevice *setto ) 
{
    m_socket = setto;
}

