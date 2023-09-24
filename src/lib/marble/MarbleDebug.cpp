// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#include "MarbleDebug.h"

#include <QIODevice>

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

    qint64 readData( char * /*data*/, qint64 /*maxSize*/ ) override
    {
        return -1;
    }

    qint64 writeData( const char * /*data*/, qint64 maxSize ) override
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
