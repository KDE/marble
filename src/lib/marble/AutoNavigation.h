//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
//

#ifndef MARBLE_AUTONAVIGATION_H
#define MARBLE_AUTONAVIGATION_H

#include "marble_export.h"
#include "MarbleGlobal.h"

#include <QObject>

namespace Marble
{

class GeoDataCoordinates;
class MarbleModel;
class ViewportParams;

class MARBLE_EXPORT AutoNavigation : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param widget the marble widget. It cannot be null.
     * @param parent optional parent object
     */
    explicit AutoNavigation( MarbleModel *model, const ViewportParams *viewport, QObject *parent = 0 );

    /** Destructor */
    ~AutoNavigation();

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

     AutoNavigation::CenterMode recenterMode() const;

     bool autoZoom() const;

public Q_SLOTS:

    /**
     * @brief For adjusting the gps location (recentering) or map(autozooming)
     * @param position current gps location
     * @param speed of the gps device
     */
     void adjust( const GeoDataCoordinates &position, qreal speed );

    /**
     * Temporarily inhibits auto-centering and auto-zooming
     */
    void inhibitAutoAdjustments();

Q_SIGNALS:
    /**
     * signal emitted when auto center is turned on (Always re-center, re-center when required ) or off(Disabled)
     * @param recenterMode the mode for re-centering selected
     */
     void recenterModeChanged( AutoNavigation::CenterMode mode );

    /**
     * signal emitted when auto zoom is toggled
     */
     void autoZoomToggled( bool enabled );

    void zoomIn( FlyToMode );

    void zoomOut( FlyToMode );

    void centerOn( const GeoDataCoordinates &position, bool animated );

private:
    class Private;
    Private * const d;
};
} //namespace marble

#endif // MARBLE_AUTONAVIGATION_H
