/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "tirexbackend.h"

#include <QTimer>
#include <QUdpSocket>

#include <cstdlib>
#include <cstring>
#include <unistd.h>

TirexBackend::TirexBackend(QObject *parent)
    : QObject(parent)
    , m_commandSocket(new QUdpSocket(this))
    , m_heartbeatTimer(new QTimer(this))
{
    // setup command socket
    const auto socketFd = getenv("TIREX_BACKEND_SOCKET_FILENO");
    if (!socketFd) {
        qFatal("TIREX_BACKEND_SOCKET_FILENO not set!");
    }
    const auto cmdSocketResult = m_commandSocket->setSocketDescriptor(std::atoi(socketFd));
    if (!cmdSocketResult) {
        qFatal("Failed to use command socket fd: %s", qPrintable(m_commandSocket->errorString()));
    }
    connect(m_commandSocket, &QUdpSocket::readyRead, this, &TirexBackend::commandReadyRead);

    // setup heartbeat pipe and timer
    const auto pipeFd = getenv("TIREX_BACKEND_PIPE_FILENO");
    const auto aliveTimeout = getenv("TIREX_BACKEND_ALIVE_TIMEOUT");
    if (pipeFd && aliveTimeout) {
        m_heartbeatFd = std::atoi(pipeFd);
        m_heartbeatTimer->setInterval(std::chrono::seconds(std::atoi(aliveTimeout)));
        m_heartbeatTimer->setSingleShot(false);
        connect(m_heartbeatTimer, &QTimer::timeout, this, [this]() {
            write(m_heartbeatFd, "alive", 5);
        });
        m_heartbeatTimer->start();
    } else {
        qWarning() << "not using heartbeat timer";
    }
}

TirexBackend::~TirexBackend() = default;

void TirexBackend::commandReadyRead()
{
    while (m_commandSocket->hasPendingDatagrams()) {
        const auto dgram = m_commandSocket->receiveDatagram();

        TirexMetatileRequest req;
        int nextIdx = 0;
        const char *type = nullptr;
        int typeLen = 0;

        while (nextIdx < dgram.data().size() && nextIdx >= 0) {
            const auto endIdx = dgram.data().indexOf('\n', nextIdx);
            if (endIdx < 0) {
                break;
            }
            const auto midIdx = dgram.data().indexOf('=', nextIdx);
            if (midIdx < 0 || midIdx >= endIdx) {
                break;
            }

            const auto key = dgram.data().constData() + nextIdx;
            const auto keyLen = midIdx - nextIdx;
            const auto value = dgram.data().constData() + midIdx + 1;
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
            auto msg = dgram.makeReply(errorMsg);
            m_commandSocket->writeDatagram(msg);
            continue;
        }

        req.reply = dgram.makeReply({});
        emit tileRequested(req);
    }
}

void TirexBackend::tileDone(const TirexMetatileRequest &req)
{
    // TODO
}

void TirexBackend::tileError(const TirexMetatileRequest &req, const QString &errMsg)
{
    QByteArray msg = "id=" + req.id + "\ntype=metatile_render_request\nresult=error\nerrmsg=" + errMsg.toUtf8() + "\n";
    auto reply = std::move(req.reply);
    reply.setData(msg);
    m_commandSocket->writeDatagram(reply);
}
