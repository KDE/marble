//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009    Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLE_MARBLELOCALE_P_H
#define MARBLE_MARBLELOCALE_P_H

#include "MarbleLocale.h"

namespace Marble
{


class MarbleLocalePrivate
{
 public:
    MarbleLocalePrivate();
    virtual ~MarbleLocalePrivate();

    MarbleLocale::MeasurementSystem m_measurementSystem;
};

}

#endif
