//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef BBCITEMGETTER_H
#define BBCITEMGETTER_H

// Marble
#include "AbstractWorkerThread.h"
#include "GeoDataLatLonBox.h"

// Qt
#include <QList>
#include <QMutex>
#include <QThread>

namespace Marble
{

class BBCStation;

class BBCItemGetter : public AbstractWorkerThread
{
    Q_OBJECT

 public:
    explicit BBCItemGetter( QObject *parent = 0 );
    ~BBCItemGetter();

    void setSchedule( const GeoDataLatLonBox& box,
                      qint32 number );

    void setStationList( const QList<BBCStation>& items );

    BBCStation station( const QString &id );

 protected:
    bool workAvailable();
    void work();

 Q_SIGNALS:
    void foundStation( const BBCStation& );

 public:
    QList<BBCStation> m_items;
    QMutex m_scheduleMutex;
    GeoDataLatLonBox m_scheduledBox;
    qint32 m_scheduledNumber;
};

} // namespace Marble

#endif // BBCITEMGETTER_H
