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

#include <QtCore/QDebug>

#include "marble_export.h"

namespace Marble
{
/**
  * a simple null device
  */
class NullDevice : public QIODevice
{
    public:
        qint64 readData( char * /*data*/, qint64 /*maxSize*/ ) { return -1; };
        qint64 writeData( const char * /*data*/, qint64 maxSize ) { return maxSize; };
};

/**
  * a class which takes all the settings and exposes them
  */

class MARBLE_EXPORT MarbleDebug
{
    public:
        static bool enable;
        static QIODevice* nullDevice() { static QIODevice *device = new NullDevice; return device; };
};

/**
  * an inline function which should replace qDebug()
  */

inline QDebug mDebug()
{
    if ( MarbleDebug::enable )
        return QDebug( QtDebugMsg );
    else
        return QDebug( MarbleDebug::nullDevice() );
}

} // namespace Marble

#endif
