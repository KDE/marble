//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_MARBLEDEBUG_H
#define MARBLE_MARBLEDEBUG_H

#include <QDebug>

#include "marble_export.h"

namespace Marble
{

/**
  * a class which takes all the settings and exposes them
  */
class MARBLE_EXPORT MarbleDebug
{
public:
    /**
     * @brief isEnabled returns whether debug information output is generated
     */
    static bool isEnabled();

    /**
     * @brief setEnabled Toggle debug information output generation
     * @param enabled Set to true to enable debug output, false to disable
     */
    static void setEnabled(bool enabled);

private:
    static bool m_enabled;

};

/**
  * a function to replace qDebug() in Marble library code
  */
MARBLE_EXPORT QDebug mDebug();

} // namespace Marble

#endif
