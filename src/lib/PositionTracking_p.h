//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
// Copyright 2010   Thibaut Gridel  <tgridel@free.fr>
//

#ifndef MARBLE_POSITIONTRACKING_P_H
#define MARBLE_POSITIONTRACKING_P_H

#include "PositionTracking.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTreeModel.h"
#include "AbstractProjection.h"
#include "MarbleModel.h"
#include "MarbleMath.h"
#include "MarbleDebug.h"
#include "PositionProviderPlugin.h"

namespace Marble
{
class PositionTrackingPrivate : public QObject
{
    Q_OBJECT
    public:
    PositionTrackingPrivate( GeoDataTreeModel *model )
        : QObject( model ),
        m_document( 0 ),
        m_treeModel( model ),
        m_positionProvider( 0 )
    {
    }

    public Q_SLOTS:
    void setPosition( GeoDataCoordinates position,
                      GeoDataAccuracy accuracy );

    void setStatus( PositionProviderStatus status );

    Q_SIGNALS:
    void  gpsLocation( GeoDataCoordinates, qreal );

    void statusChanged( PositionProviderStatus status );

    public:
    GeoDataDocument     *m_document;
    GeoDataTreeModel    *m_treeModel;

    GeoDataCoordinates  m_gpsCurrentPosition;
    GeoDataCoordinates  m_gpsPreviousPosition;
    GeoDataLineString  *m_currentLineString;

    PositionProviderPlugin* m_positionProvider;

    GeoDataAccuracy m_accuracy;
};
}


#endif
