//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#include "MarbleDebug.h"

namespace Marble
{
bool MarbleDebug::m_enabled = false;

class NullDevice : public QIODevice
{
public:
    NullDevice()
    {
        open( QIODevice::WriteOnly );
    }

    qint64 readData( char * /*data*/, qint64 /*maxSize*/ )
    {
        return -1;
    }

    qint64 writeData( const char * /*data*/, qint64 maxSize )
    {
        return maxSize;
    }
};

QDebug mDebug()
{
    if ( MarbleDebug::isEnabled() ) {
        return QDebug( QtDebugMsg );
    }
    else {
        static QIODevice *device = new NullDevice;
        return QDebug( device );
    }
}

bool MarbleDebug::isEnabled()
{
    return MarbleDebug::m_enabled;
}

void MarbleDebug::setEnabled(bool enabled)
{
    MarbleDebug::m_enabled = enabled;
}

} // namespace Marble
