//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn  <rahn@kde.org>"
//

#ifndef MARBLE_PHYSICS_H
#define MARBLE_PHYSICS_H

#include "GeoDataPoint.h"
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QTimer>
#include <QtCore/QTimeLine>

class MarblePhysics : public QObject
{
 Q_OBJECT

 public:
    MarblePhysics();
    ~MarblePhysics();

    void jumpTo( const GeoDataPoint &targetPoint );
    GeoDataPoint suggestedPosition() const;

 Q_SIGNALS:
    void valueChanged( qreal );

 public Q_SLOTS:

    void setCurrentPosition( const GeoDataPoint &currentPostion );

 private:
    GeoDataPoint m_sourcePosition;
    GeoDataPoint m_targetPosition;

    QTimeLine *m_timeLine;

    int    m_jumpDuration;
};

#endif // MARBLE_PHYSICS_H
