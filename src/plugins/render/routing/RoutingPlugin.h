//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
//

#ifndef ROUTINGPLUGIN_H
#define ROUTINGPLUGIN_H

#define QT_USE_FAST_CONCATENATION

#include "AbstractFloatItem.h"
#include "GeoDataCoordinates.h"
#include "PositionProviderPlugin.h"
#include "global.h"

#include <QtCore/QObject>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QLabel>

namespace Ui
{
    class RoutingWidgetSmall;
    class RoutingInformationWidget;
    class RoutingInformationWidgetSmall;
}

namespace Marble
{
class MarbleWidget;
class AdjustNavigation;
class RoutingModel;
class WidgetGraphicsItem;
class RoutingPlugin : public AbstractFloatItem
{

Q_OBJECT
Q_INTERFACES( Marble::RenderPluginInterface )
MARBLE_PLUGIN( RoutingPlugin )

public:
    explicit RoutingPlugin( const QPointF &point = QPointF( -10, -10 ) );

    ~RoutingPlugin();

    /**
    * An enum type
    * Represents which recentering method is selected
    */
    enum CenterMode{
            Disabled = 0,          /**< Enum Value Disabled. Disable Re-Centering */
            AlwaysRecenter = 1,    /**< Enum Value AlwaysRecenter. Re-center always to the map center */
            RecenterOnBorder = 2   /**< Enum Value RecenterOnBorder. Re-center when reaching map border */
    };


    QStringList backendTypes() const;

    void initialize();

    bool isInitialized() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon() const;

    bool eventFilter(QObject *object, QEvent *e);

private Q_SLOTS:

    /**
     * update controls when map theme is changed
     */
     void selectTheme( const QString &theme);

    /**
     * shows routing item for small screen devices
     */
     void showRoutingItem( bool show );

    /**
     * sets time and distance remaining to reach the destination
     */
     void setDestinationInformation( qint32, qreal );

    /**
     * sets current position of the gps device
     */
     void setCurrentLocation( GeoDataCoordinates, qreal );

    /**
     * disables the navigation menu if Position Tracking is disabled
     */
     void updateButtonStates( PositionProviderPlugin *activePlugin );

     /** Activate or deactivate position tracking */
     void togglePositionTracking( bool enabled );

     /** Switch source/destination of the route and recalculate it */
     void reverseRoute();

private:

    /**
     * Enable/disable zoom in/out buttons
     */
     void updateButtons( int zoomValue );

    /**
     * updates the information in the routing item with relevant information
     */
     void updateRoutingItem();

    /**
     * sets the distance of the next instruction as well as the next instruction text on the
     * routing information item
     */
     void updateInstructionLabel( QLabel *label );

     void updateWidget();

     MarbleWidget                        *m_marbleWidget;
     WidgetGraphicsItem                  *m_widgetItem;
     WidgetGraphicsItem                  *m_widgetItemRouting;
     MarbleGlobal::Profiles               m_profiles;
     RoutingModel                        *m_routingModel;
     AdjustNavigation                    *m_adjustNavigation;
     GeoDataCoordinates                   m_currentPosition;
     qreal                                m_currentSpeed;
     qint32                               m_remainingTime;
     qreal                                m_remainingDistance;
     Ui::RoutingWidgetSmall              *m_routingWidgetSmall;
     Ui::RoutingInformationWidget        *m_routingInformationWidget;
     Ui::RoutingInformationWidgetSmall   *m_routingInformationWidgetSmall;
};

}

#endif // ROUTINGPLUGIN_H
