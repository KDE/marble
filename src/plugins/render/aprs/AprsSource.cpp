//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
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

