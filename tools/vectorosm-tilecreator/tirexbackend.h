/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TIREXBACKEND_H
#define TIREXBACKEND_H

#include <QElapsedTimer>
#include <QNetworkDatagram>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>

/** A Tirex meta tile. */
class TirexMetatile
{
public:
    int x = -1;
    int y = -1;
    int z = -1;
};

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

    /** Report a rendered request as done to Tirex. */
    void tileDone(const TirexMetatileRequest &req);
    /** Indicate a render request failed. */
    void tileError(const TirexMetatileRequest &req, const QString &errMsg);

    /** Returns the value of the entry @p key from the corresponding map configuration file of Tirex. */
    QVariant configValue(const QString &key) const;
    /** Returns the full file name of the requested meta tile. */
    QString metatileFileName(const TirexMetatileRequest &req);

    /** Amount of rows in a single metatile. */
    int metatileRows() const;
    /** Amount of columns in a single metatile. */
    int metatileColumns() const;

    /** Writes the meta tile header structures.
     *  QIODevice is positioned at the end for writing the first content tile afterwards.
     */
    void writeMetatileHeader(QIODevice *io, const TirexMetatile &tile) const;
    /** Updates the tile offset and size at the given index.
     * The QIODevice seek position will not change by this.
     */
    void writeMetatileEntry(QIODevice *io, int entryIdx, int offset, int size) const;

Q_SIGNALS:
    /** Emitted when a new tile is requested.
     *  Respond to this by calling tileDone or tileError.
     */
    void tileRequested(const TirexMetatileRequest &req);

private:
    void commandReadyRead();

    QUdpSocket m_commandSocket;
    QTimer m_heartbeatTimer;
    QString m_tileDir;
    QElapsedTimer m_renderTime;
    int m_heartbeatFd = -1;
    static constexpr const int m_metatileRows = 8; // TODO read from config eventually
    static constexpr const int m_metatileCols = 8;
};

#endif // TIREXBACKEND_H
