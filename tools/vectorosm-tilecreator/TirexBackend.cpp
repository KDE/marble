/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TirexBackend.h"

#include <QDir>
#include <QScopedValueRollback>
#include <QSettings>
#include <QSocketNotifier>

#include <cstdlib>
#include <cstring>
#include <unistd.h>

TirexBackend::TirexBackend(QObject *parent)
    : QObject(parent)
{
    // setup command socket
    const auto socketFd = getenv("TIREX_BACKEND_SOCKET_FILENO");
    if (socketFd) {
        m_commandSocketFd = std::atoi(socketFd);
        m_socketNotifier = new QSocketNotifier(m_commandSocketFd, QSocketNotifier::Read, this);
        connect(m_socketNotifier, &QSocketNotifier::activated, this, &TirexBackend::commandReadyRead);
    } else {
        qFatal("TIREX_BACKEND_SOCKET_FILENO not set!");
    }

    // setup heartbeat pipe and timer
    const auto pipeFd = getenv("TIREX_BACKEND_PIPE_FILENO");
    const auto aliveTimeout = getenv("TIREX_BACKEND_ALIVE_TIMEOUT");
    if (pipeFd && aliveTimeout) {
        m_heartbeatFd = std::atoi(pipeFd);
        m_heartbeatTimer.setInterval(std::chrono::seconds(std::atoi(aliveTimeout)));
        m_heartbeatTimer.setSingleShot(false);
        connect(&m_heartbeatTimer, &QTimer::timeout, this, [this]() {
            write(m_heartbeatFd, "alive", 5);
        });
        m_heartbeatTimer.start();
    } else {
        qWarning() << "not using heartbeat timer";
    }

    // read map configuration
    m_tileDir = configValue(QStringLiteral("tiledir")).toString();
    if (m_tileDir.isEmpty()) {
        m_tileDir = QStringLiteral("output/");
    }
}

TirexBackend::~TirexBackend() = default;

void TirexBackend::commandReadyRead()
{
    if (m_recursionLock) {
        return;
    }
    QScopedValueRollback<bool> lock(m_recursionLock, true);

    while (true) {
        m_renderTime.restart();
        TirexMetatileRequest req;
        req.addrSize = sizeof(req.addr);
        bzero(&req.addr, req.addrSize);
        QByteArray data(0xffff, 0);
        auto n = recvfrom(m_commandSocketFd, data.data(), 0xffff, MSG_DONTWAIT, reinterpret_cast<sockaddr *>(&req.addr), &req.addrSize);
        if (n <= 0) {
            break;
        }
        data.resize(n);

        int nextIdx = 0;
        const char *type = nullptr;
        int typeLen = 0;

        while (nextIdx < data.size() && nextIdx >= 0) {
            const auto endIdx = data.indexOf('\n', nextIdx);
            if (endIdx < 0) {
                break;
            }
            const auto midIdx = data.indexOf('=', nextIdx);
            if (midIdx < 0 || midIdx >= endIdx) {
                break;
            }

            const auto key = data.constData() + nextIdx;
            const auto keyLen = midIdx - nextIdx;
            const auto value = data.constData() + midIdx + 1;
            const auto valueLen = endIdx - midIdx - 1;

            if (keyLen == 4 && std::strncmp(key, "type", 4) == 0) {
                type = value;
                typeLen = valueLen;
            } else if (keyLen == 2 && std::strncmp(key, "id", 2) == 0) {
                req.id = QByteArray(value, valueLen);
            } else if (keyLen == 1 && std::strncmp(key, "x", 1) == 0) {
                req.tile.x = std::atoi(value);
            } else if (keyLen == 1 && std::strncmp(key, "y", 1) == 0) {
                req.tile.y = std::atoi(value);
            } else if (keyLen == 1 && std::strncmp(key, "z", 1) == 0) {
                req.tile.z = std::atoi(value);
            } else if (keyLen == 3 && std::strncmp(key, "map", 3) == 0) {
                req.map = QByteArray(value, valueLen);
            }

            nextIdx = endIdx + 1;
        }

        if (std::strncmp(type, "metatile_render_request\n", 24) != 0) {
            QByteArray errorMsg;
            errorMsg += "id=" + req.id + "\n";
            errorMsg += "type=" + QByteArray(type, typeLen) + "\n";
            errorMsg += "result=error\nerrormsg=unsupported requested\n";
            sendto(m_commandSocketFd, errorMsg.constData(), errorMsg.size(), 0, reinterpret_cast<const sockaddr *>(&req.addr), req.addrSize);
            continue;
        }

        emit tileRequested(req);
    }
}

void TirexBackend::tileDone(const TirexMetatileRequest &req)
{
    QByteArray msg = "id=" + req.id + "\ntype=metatile_render_request\nresult=ok\nx=" + QByteArray::number(req.tile.x) + "\ny=" + QByteArray::number(req.tile.y)
        + "\nz=" + QByteArray::number(req.tile.z) + "\nmetatile=" + metatileFileName(req).toUtf8()
        + "\nrender_time=" + QByteArray::number(m_renderTime.elapsed()) + "\n";
    sendto(m_commandSocketFd, msg.constData(), msg.size(), 0, reinterpret_cast<const sockaddr *>(&req.addr), req.addrSize);
}

void TirexBackend::tileError(const TirexMetatileRequest &req, const QString &errMsg)
{
    QByteArray msg = "id=" + req.id + "\ntype=metatile_render_request\nresult=error\nerrmsg=" + errMsg.toUtf8() + "\n";
    sendto(m_commandSocketFd, msg.constData(), msg.size(), 0, reinterpret_cast<const sockaddr *>(&req.addr), req.addrSize);
}

QVariant TirexBackend::configValue(const QString &key) const
{
    const auto configFiles = getenv("TIREX_BACKEND_MAP_CONFIGS");
    if (configFiles) {
        QSettings settings(QString::fromUtf8(configFiles), QSettings::IniFormat);
        return settings.value(key);
    }
    return {};
}

QString TirexBackend::metatileFileName(const TirexMetatileRequest &req)
{
    auto x = req.tile.x;
    auto y = req.tile.y;
    uint8_t hash[5];
    for (auto i = 0; i < 5; i++) {
        hash[i] = ((x & 0x0f) << 4) | (y & 0x0f);
        x >>= 4;
        y >>= 4;
    }

    QString path = m_tileDir + QLatin1Char('/') + QString::number(req.tile.z) + QLatin1Char('/') + QString::number(hash[4]) + QLatin1Char('/')
        + QString::number(hash[3]) + QLatin1Char('/') + QString::number(hash[2]) + QLatin1Char('/') + QString::number(hash[1]) + QLatin1Char('/');
    QDir().mkpath(path);
    path += QString::number(hash[0]) + QLatin1String(".meta");
    return path;
}

int TirexBackend::metatileColumns() const
{
    return m_metatileCols;
}

int TirexBackend::metatileRows() const
{
    return m_metatileRows;
}

struct TirexMetatileHeader {
    char magic[4];
    int count;
    int x;
    int y;
    int z;
};

struct TirexMetatileEntry {
    int offset;
    int size;
};

void TirexBackend::writeMetatileHeader(QIODevice *io, const TirexMetatile &tile) const
{
    TirexMetatileHeader header;
    header.magic[0] = 'M';
    header.magic[1] = 'E';
    header.magic[2] = 'T';
    header.magic[3] = 'A';
    header.count = m_metatileRows * m_metatileCols;
    header.x = tile.x;
    header.y = tile.y;
    header.z = tile.z;
    io->write(reinterpret_cast<const char *>(&header), sizeof(header));

    TirexMetatileEntry entry;
    entry.offset = 0;
    entry.size = 0;
    for (int i = 0; i < header.count; ++i) {
        io->write(reinterpret_cast<const char *>(&entry), sizeof(entry));
    }
}

void TirexBackend::writeMetatileEntry(QIODevice *io, int entryIdx, int offset, int size) const
{
    const auto seekPos = io->pos();

    io->seek(sizeof(TirexMetatileHeader) + entryIdx * sizeof(TirexMetatileEntry));
    TirexMetatileEntry entry;
    entry.offset = offset;
    entry.size = size;
    io->write(reinterpret_cast<const char *>(&entry), sizeof(TirexMetatileEntry));

    io->seek(seekPos);
}

#include "moc_TirexBackend.cpp"
