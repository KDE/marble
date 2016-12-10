//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef TILEQUEUE_H
#define TILEQUEUE_H

#include "TileId.h"

#include <QSharedMemory>

namespace Marble {

QDataStream& operator<<(QDataStream&, const TileId& tile);
QDataStream& operator>>(QDataStream&, TileId& tile);

class TileQueue
{

public:
    TileQueue();
    ~TileQueue();

    bool read(QSet<TileId> &tileQueue);
    bool write(const QSet<TileId> &tileQueue);

private:
    void release();

    Q_DISABLE_COPY(TileQueue)

    QSharedMemory m_sharedMemory;
};

}

#endif
