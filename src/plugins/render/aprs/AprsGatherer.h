//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSGATHERER_H
#define APRSGATHERER_H

#include <QThread>
#include <QMap>
#include <QString>

#include "AprsSource.h"
#include "AprsObject.h"

class QIODevice;
class QMutex;

namespace Marble {
        
    class AprsGatherer : public QThread
    {
        Q_OBJECT

            public:
        AprsGatherer( AprsSource *from,
                      QMap<QString, AprsObject *> *objects,
                      QMutex *mutex,
                      QString *filter
            );
        AprsGatherer( QIODevice                   *from,
                      QMap<QString, AprsObject *> *objects,
                      QMutex *mutex,
                      QString *filter
            );
        void run();

        void addObject( const QString &callSign,
                        qreal latitude, qreal longitude, bool canDoDirect,
                        const QString &routePath,
                        const QChar &symbolTable,
                        const QChar &symbolCode );

        void setDumpOutput( bool to );
        bool dumpOutput() const;

        void setSeenFrom ( GeoAprsCoordinates::SeenFrom seenFrom );
        GeoAprsCoordinates::SeenFrom seenFrom();

        void shutDown();
        static void sleepFor(int seconds);

      private:
        void initMicETables();
        static qreal calculateLongitude( const QString &threeBytes,
                                         int offset, bool isEast );

        AprsSource                  *m_source;
        QIODevice                   *m_socket;
        QString                     *m_filter;
        bool                         m_running;
        bool                         m_dumpOutput;
        GeoAprsCoordinates::SeenFrom m_seenFrom;
        QString                      m_sourceName;

        // Shared with the parent thread
        QMutex                      *m_mutex;
        QMap<QString, AprsObject *> *m_objects;

        QMap<QPair<QChar, QChar>, QString> m_pixmaps;

        // Mic-E decoding tables
        QMap<QChar, int>                   m_dstCallDigits;
        QMap<QChar, bool>                  m_dstCallSouthEast;
        QMap<QChar, int>                   m_dstCallLongitudeOffset;
        QMap<QChar, int>                   m_dstCallMessageBit;

        QMap<int, QString>                 m_standardMessageText;
        QMap<int, QString>                 m_customMessageText;
        QMap<QChar, int>                   m_infoFieldByte1;
    };
}

#endif /* APRSGATHERER_H */
