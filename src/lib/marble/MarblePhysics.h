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

#include <QObject>

#include "MarbleGlobal.h"

namespace Marble
{

class MarblePhysicsPrivate;
class MarbleAbstractPresenter;
class GeoDataLookAt;

class MarblePhysics : public QObject
{
 Q_OBJECT

 public:
    /**
      * @brief Constructor
      * @param presenter the MarbleWidget that is being animated
      */
    explicit MarblePhysics( MarbleAbstractPresenter *presenter );

    /**
      * @brief Destructor
      */
    ~MarblePhysics() override;

    /**
      * @brief Initiate an animation to the target according to the given mode.
      * @param target camera position indicating the target of the animation
      * @param mode animation mode; @code Instant @endcode means no animation
      */
    void flyTo( const GeoDataLookAt &target, FlyToMode mode = Instant );

private Q_SLOTS:
    void updateProgress(qreal progress);

    /**
      * @brief Switch to still mode when an animation is finished
      */
    void startStillMode();

 private:
    Q_DISABLE_COPY( MarblePhysics )

    MarblePhysicsPrivate* d;
};

}

#endif
