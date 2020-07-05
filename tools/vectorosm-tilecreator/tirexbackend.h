/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TIREXBACKEND_H
#define TIREXBACKEND_H

#include "tirexmetatile.h"

#include <QNetworkDatagram>
#include <QObject>

class TirexMetatile;
class QUdpSocket;
class QTimer;

/** A render request for a meta-tile from Tirex. */
class TirexMetatileRequest
{
public:
    TirexMetatile tile;
    QByteArray id;
    QByteArray map;
    QNetworkDatagram reply;
};


/** Interface to Tirex.
 *  @see https://wiki.openstreetmap.org/wiki/Tirex/Backends
 */
class TirexBackend : public QObject
{
    Q_OBJECT
public:
    explicit TirexBackend(QObject *parent = nullptr);
    ~TirexBackend();

    void tileDone(const TirexMetatileRequest &req);
    void tileError(const TirexMetatileRequest &req, const QString &errMsg);

Q_SIGNALS:
    void tileRequested(const TirexMetatileRequest &tile);

private:
    void commandReadyRead();

    QUdpSocket *m_commandSocket = nullptr;
    QTimer *m_heartbeatTimer = nullptr;
    int m_heartbeatFd = -1;
};

#endif // TIREXBACKEND_H
