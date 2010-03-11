/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARBLE_ROUTING_WIDGET_H
#define MARBLE_ROUTING_WIDGET_H

#include <GeoDataCoordinates.h>
#include <GeoDataLineString.h>
#include "RoutingManager.h"

#include <QtGui/QWidget>
#include <QtCore/QModelIndex>

namespace Marble
{

class MarbleWidget;
class RoutingInputWidget;
class RoutingWidgetPrivate;

/**
  * A widget consisting of input fields for places / routing destinations,
  * a list view showing routing instructions and a interactive paint layer
  * showing placemarks and the route
  */
class RoutingWidget : public QWidget
{
    Q_OBJECT

 public:
    /**
      * @brief Constructor
      * @param marbleWidget The marble widget used to register an event handler
      * on. Must not be null.
      * @param parent Optional parent widget
      */
    explicit RoutingWidget(MarbleWidget* marbleWidget, QWidget *parent);

    /** Destructor */
    ~RoutingWidget();

private Q_SLOTS:
    /** Retrieve route directions */
    void retrieveRoute();

    /** User activated a route instruction element in the list view, synchronize paint layer */
    void activateItem ( const QModelIndex &index );

    /** Placemark search is finished in an input field, switch to its placemark model */
    void handleSearchResult(RoutingInputWidget* widget);

    /** Switch to the placemark model of an input field */
    void activateInputWidget(RoutingInputWidget* widget);

    /** A placemark was selected in the map, synchronize list view */
    void activatePlacemark(const QModelIndex &index );

    /** Add another input field at the end */
    void addInputWidget();

    /** Remove an existing input field */
    void removeInputWidget(RoutingInputWidget* widget);

    /** Route state changed */
    void updateRouteState(RoutingManager::State state, const GeoDataLineString &route);

    /** An input field requests a position input from the map */
    void requestMapPosition(RoutingInputWidget* widget);

    /** Position in the map selected by the user after a previous slotMapInputRequested */
    void retrieveSelectedPoint(const GeoDataCoordinates &coordinates);

    /** Update the text of the Search / GetDirections button */
    void adjustSearchButton();

private:
    RoutingWidgetPrivate* const d;
};

} // namespace Marble

#endif // MARBLE_ROUTING_WIDGET_H
