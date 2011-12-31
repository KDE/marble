//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "jobparameters.h"

JobParameters::JobParameters()
{
    // nothing to do
}

QDir JobParameters::base()
{
    return m_base;
}

void JobParameters::setBase(const QDir &dir)
{
    m_base = dir;
}
