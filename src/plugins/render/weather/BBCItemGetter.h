//
// This file is part of the Marble Desktop Globe.
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
#include "GeoDataLatLonAltBox.h"

// Qt
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QThread>

namespace Marble
{

class AbstractDataPluginItem;
class BBCWeatherItem;
class MarbleDataFacade;

class BBCItemGetter : public AbstractWorkerThread
{
    Q_OBJECT

 public:
    BBCItemGetter( QObject *parent = 0 );
    ~BBCItemGetter();

    void setSchedule( const GeoDataLatLonAltBox& box,
                      MarbleDataFacade *facade,
                      qint32 number );

    void setStationList( const QList<BBCWeatherItem*>& items );

 protected:
    bool workAvailable();
    void work();

 Q_SIGNALS:
    void requestedDownload( const QUrl& url,
                            const QString& type,
                            AbstractDataPluginItem *item );

 public:
    QList<BBCWeatherItem*> m_items;
    QMutex m_scheduleMutex;
    GeoDataLatLonAltBox m_scheduledBox;
    qint32 m_scheduledNumber;
};

} // namespace Marble

#endif // BBCITEMGETTER_H
