//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTINGWIDGET_H
#define MARBLE_ROUTINGWIDGET_H

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "RoutingManager.h"
#include "marble_export.h"

#include <QtCore/QModelIndex>
#include <QtGui/QWidget>

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
class MARBLE_EXPORT RoutingWidget : public QWidget
{
    Q_OBJECT

public:
    /**
      * @brief Constructor
      * @param marbleWidget The marble widget used to register an event handler
      * on. Must not be null.
      * @param parent Optional parent widget
      */
    explicit RoutingWidget( MarbleWidget *marbleWidget, QWidget *parent );

    /** Destructor */
    ~RoutingWidget();

    /**
      * Toggle offline mode. In offline mode, new routes cannot be downloaded
      */
    void setWorkOffline( bool offline );

    /** Show or hide the "open file..." button. Default is false (not visible) */
    void setShowDirectionsButtonVisible( bool visible );

public Q_SLOTS:
    /** Ask the user for a kml file to open */
    void openRoute();

    /** Ask the user for a kml file to save the current route to */
    void saveRoute();

private Q_SLOTS:
    /** Retrieve route directions */
    void retrieveRoute();

    /** User activated a route instruction element in the list view, synchronize paint layer */
    void activateItem ( const QModelIndex &index );

    /** Placemark search is finished in an input field, switch to its placemark model */
    void handleSearchResult( RoutingInputWidget *widget );

    /** Switch to the placemark model of an input field */
    void centerOnInputWidget( RoutingInputWidget *widget );

    /** A placemark was selected in the map, synchronize list view */
    void activatePlacemark( const QModelIndex &index );

    /** Insert another input field at the given position */
    void insertInputWidget( int index );

    /** Add another input field at the end */
    void addInputWidget();

    /** Remove an existing input field */
    void removeInputWidget( RoutingInputWidget *widget );

    /** Remove an existing input field */
    void removeInputWidget( int index );

    /** Route state changed */
    void updateRouteState( RoutingManager::State state, RouteRequest *route );

    /** An input field requests a position input from the map */
    void requestMapPosition( RoutingInputWidget *widget, bool enabled );

    /** Position in the map selected by the user after a previous slotMapInputRequested */
    void retrieveSelectedPoint( const GeoDataCoordinates &coordinates );

    /** Update the text of the Search / GetDirections button */
    void adjustSearchButton();

    /** The user canceled point selection from the map */
    void pointSelectionCanceled();

    /** show profile configuration dialog */
    void configureProfile();

    /** Progress animation update */
    void updateProgress();

    /** Switch to an alternative route */
    void switchRoute( int index );

    /** Toggle visibility of alternative routes */
    void updateAlternativeRoutes();

    /** Select the first routing profile if none is selected yet */
    void selectFirstProfile();

    /** Show the directions ListView */
    void showDirections();

    /** Update the route request with the given profile */
    void setRoutingProfile( int index );

private:
    RoutingWidgetPrivate *const d;
};

} // namespace Marble

#endif
