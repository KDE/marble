//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "TileQueue.h"

#include <QCryptographicHash>
#include <QBuffer>
#include <QDataStream>
#include <QDebug>


namespace Marble {

QString generateKeyHash(const QString& key, const QString& salt)
{
    QByteArray data;
    data.append(key.toUtf8());
    data.append(salt.toUtf8());
    data = QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();
    return data;
}

TileQueue::TileQueue()
    : m_sharedMemory(generateKeyHash("marble-vectorosm", "tile-queue"))
{
    // nothing to do
}

TileQueue::~TileQueue()
{
    release();
}

bool TileQueue::read(QSet<TileId> &tileQueue)
{
    if (!m_sharedMemory.attach()) {
        if (m_sharedMemory.error() == QSharedMemory::NotFound) {
            if (!m_sharedMemory.create(256 * 1024 * sizeof(TileId))) {
                qDebug() << "Cannot create shared memory";
                return false;
            }
        }
    }

    if (m_sharedMemory.lock()) {
        QBuffer buffer;
        QDataStream in(&buffer);
        buffer.setData((char*)m_sharedMemory.constData(), m_sharedMemory.size());
        buffer.open(QBuffer::ReadOnly);
        in >> tileQueue;
        return true;
    }
    return false;
}

bool TileQueue::write(const QSet<TileId> &tileQueue)
{
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << tileQueue;
    if (buffer.size() <= m_sharedMemory.size()) {
        memcpy(m_sharedMemory.data(), buffer.data().data(), buffer.size());
        m_sharedMemory.unlock();
        return true;
    }

    m_sharedMemory.unlock();
    return false;
}

void TileQueue::release()
{
    if (m_sharedMemory.isAttached()) {
        m_sharedMemory.unlock();
        m_sharedMemory.detach();
    }
}

QDataStream &operator<<(QDataStream &stream, const TileId &tile)
{
    stream << tile.x();
    stream << tile.y();
    stream << tile.zoomLevel();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, TileId &tile)
{
    int x, y, z;
    stream >> x;
    stream >> y;
    stream >> z;
    tile = TileId(0, z, x, y);
    return stream;
}

}
