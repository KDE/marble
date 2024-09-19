// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008-2009 Torsten Rahn <tackat@kde.org>
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
