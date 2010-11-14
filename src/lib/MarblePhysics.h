//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn  <rahn@kde.org>
//

#ifndef MARBLE_MARBLEPHYSICS_H
#define MARBLE_MARBLEPHYSICS_H

#include "GeoDataLookAt.h"

#include <QtCore/QObject>

namespace Marble
{

class MarblePhysicsPrivate;
class ViewportParams;

class MarblePhysics : public QObject
{
 Q_OBJECT

 public:
    /**
      * @brief Constructor
      * parent Pointer to the parent object
      */
    explicit MarblePhysics( QObject * parent = 0);
    
    /**
      * @brief Destructor
      */
    ~MarblePhysics();

    /**
      * @brief Calculate an interpolation between source and target according
      * to the given mode
      * @param source Camera position indicating the start point of the interpolation
      * @param target Camera position indicating the target point of the interpolation
      * @param mode Interpolation (animation) mode. Instant means no interpolation.
      * @see positionReached
      */
    void flyTo( const GeoDataLookAt &source, const GeoDataLookAt &target,
                ViewportParams *viewport, FlyToMode mode = Instant );

 Q_SIGNALS:
    /**
      * Emitted for each interpolation point between source and target after
      * flyTo was called.
      * @param position Interpolated or final camera position
      * @see flyTo
      */
    void positionReached( const GeoDataLookAt position );

    /**
      * The target was reached.
      * @see flyTo
      */
    void finished();

private Q_SLOTS:
    void updateProgress(qreal progress);
    
 private:
    Q_DISABLE_COPY( MarblePhysics )
    
    MarblePhysicsPrivate* d;
};

}

#endif
