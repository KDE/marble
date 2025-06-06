// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSGATHERER_H
#define APRSGATHERER_H

#include <QMap>
#include <QString>
#include <QThread>

#include "AprsObject.h"
#include "AprsSource.h"

class QIODevice;
class QMutex;

namespace Marble
{

class AprsGatherer : public QThread
{
    Q_OBJECT

public:
    AprsGatherer(AprsSource *from, QMap<QString, AprsObject *> *objects, QMutex *mutex, QString *filter);
    AprsGatherer(QIODevice *from, QMap<QString, AprsObject *> *objects, QMutex *mutex, QString *filter);
    void run() override;

    void addObject(const QString &callSign,
                   qreal latitude,
                   qreal longitude,
                   bool canDoDirect,
                   const QString &routePath,
                   const QChar &symbolTable,
                   const QChar &symbolCode);

    void setDumpOutput(bool to);
    bool dumpOutput() const;

    void setSeenFrom(GeoAprsCoordinates::SeenFrom seenFrom);
    GeoAprsCoordinates::SeenFrom seenFrom();

    void shutDown();
    static void sleepFor(int seconds);

private:
    void initMicETables();
    static qreal calculateLongitude(const QString &threeBytes, int offset, bool isEast);

    AprsSource *m_source = nullptr;
    QIODevice *m_socket = nullptr;
    QString *const m_filter;
    bool m_running;
    bool m_dumpOutput;
    GeoAprsCoordinates::SeenFrom m_seenFrom;
    QString m_sourceName;

    // Shared with the parent thread
    QMutex *m_mutex = nullptr;
    QMap<QString, AprsObject *> *m_objects = nullptr;

    QMap<QPair<QChar, QChar>, QString> m_pixmaps;

    // Mic-E decoding tables
    QMap<QChar, int> m_dstCallDigits;
    QMap<QChar, bool> m_dstCallSouthEast;
    QMap<QChar, int> m_dstCallLongitudeOffset;
    QMap<QChar, int> m_dstCallMessageBit;

    QMap<int, QString> m_standardMessageText;
    QMap<int, QString> m_customMessageText;
    QMap<QChar, int> m_infoFieldByte1;
};
}

#endif /* APRSGATHERER_H */
