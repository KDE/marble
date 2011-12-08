//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
//

#ifndef ADJUSTNAVIGATION_H
#define ADJUSTNAVIGATION_H

#include "marble_export.h"
#include "MarbleWidget.h"
#include "GeoDataCoordinates.h"

#include <QtGui/QWidget>

namespace Marble
{

class GeoDataCoordinates;
class MarbleWidget;
class PositionTracking;

class MARBLE_EXPORT AdjustNavigation : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param widget the marble widget. It cannot be null.
     * @param parent optional parent object
     */
    explicit AdjustNavigation( MarbleWidget *widget, QObject *parent = 0 );

    /** Destructor */
    ~AdjustNavigation();

    /**
    * An enum type
    * Represents which recentering method is selected
    */
    enum CenterMode {
            DontRecenter = 0,
            AlwaysRecenter = 1,    /**< Enum Value AlwaysRecenter. Recenter always to the map center */
            RecenterOnBorder = 2   /**< Enum Value RecenterOnBorder. Recenter when reaching map border */
    };

    /**
     * @brief For Auto Centering adjustment of map in Navigation Mode
     * @param recenterMode toggles among the recenteing method chosen
     * @see CenterMode
     */
    void setRecenter( CenterMode recenterMode );

    /**
     * @brief For Auto Zooming adjustment of map in Navigation Mode
     * @param activate true to enable auto zooming
     */
     void setAutoZoom( bool activate );

     AdjustNavigation::CenterMode recenterMode() const;

     bool autoZoom() const;

public Q_SLOTS:

    /**
     * @brief For adjusting the gps location (recentering) or map(autozooming)
     * @param position current gps location
     * @param speed of the gps device
     */
     void adjust( const GeoDataCoordinates &position, qreal speed );

Q_SIGNALS:
    /**
     * signal emitted when auto center is turned on (Always re-center, re-center when required ) or off(Disabled)
     * @param recenterMode the mode for re-centering selected
     */
     void recenterModeChanged( AdjustNavigation::CenterMode mode );

    /**
     * signal emitted when auto zoom is toggled
     */
     void autoZoomToggled( bool enabled );

 private Q_SLOTS:

     /**
       * Temporarily inhibits auto-centering and auto-zooming
       */
     void inhibitAutoAdjustments();

private:
    class Private;
    Private * const d;
};
} //namespace marble

#endif // ADJUSTNAVIGATION_H
