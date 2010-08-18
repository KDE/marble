//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
// Copyright 2010   Thibaut Gridel  <tgridel@free.fr>
//

#ifndef MARBLE_POSITIONTRACKING_H
#define MARBLE_POSITIONTRACKING_H

#include "PositionProviderPlugin.h"

#include <QtGui/QRegion>
#include <QtGui/QPolygonF>
#include <QtCore/QObject>
#include <QtCore/QTemporaryFile>
#include <QtNetwork/QHttp>

namespace Marble
{

class GeoDataDocument;
class GeoDataCoordinates;
class FileManager;
class PluginManager;


class PositionTracking : public QObject
{
    Q_OBJECT

public:

    explicit PositionTracking( FileManager *fileManager,
                          QObject *parent = 0 );
    ~PositionTracking();

    /**
      * Change the position provider to use. You can provide 0 to disable
      * position tracking. Ownership of the provided plugin is taken.
      */
    void setPositionProviderPlugin( PositionProviderPlugin* plugin );

    QString error() const;

    /**
     * @brief provides speed of the gps device
     */
    qreal speed() const;

    /**
     * @brief provides direction of the gps device in degrees with geographical north
     */
    qreal direction() const;

Q_SIGNALS:
    void  gpsLocation( GeoDataCoordinates, qreal );

    void statusChanged( PositionProviderStatus status );

    /**
     * @brief emits positionProviderPluginChanged(0) when provider is disabled
     */

    void positionProviderPluginChanged( PositionProviderPlugin *activePlugin );

public slots:
    void setPosition( GeoDataCoordinates position,
                          GeoDataAccuracy accuracy );

    void setStatus( PositionProviderStatus status );

 private:

    GeoDataDocument     *m_document;
    FileManager         *m_fileManager;

    GeoDataCoordinates  m_gpsCurrentPosition;
    GeoDataCoordinates  m_gpsPreviousPosition;

    PositionProviderPlugin* m_positionProvider;
};

}



#endif
