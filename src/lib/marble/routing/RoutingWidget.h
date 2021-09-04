// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGWIDGET_H
#define MARBLE_ROUTINGWIDGET_H

#include "RoutingManager.h"
#include "marble_export.h"

#include <QWidget>

class QModelIndex;

namespace Marble
{

class MarbleWidget;
class RoutingInputWidget;
class RoutingWidgetPrivate;
class RouteSyncManager;
class GeoDataCoordinates;

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
    ~RoutingWidget() override;

    /** Show or hide the "open file..." button. Default is false (not visible) */
    void setShowDirectionsButtonVisible( bool visible );

    void setRouteSyncManager(RouteSyncManager* manager);

public Q_SLOTS:
    /** Ask the user for a kml file to open */
    void openRoute();

    /** Ask the user for a kml file to save the current route to */
    void saveRoute();

    /** Upload route to the cloud */
    void uploadToCloud();

    /** Open cloud routes dialog */
    void openCloudRoutesDialog();

    /** Add another input field at the end */
    void addInputWidget();

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

    /** Remove an existing input field */
    void removeInputWidget( RoutingInputWidget *widget );

    /** Remove an existing input field */
    void removeInputWidget( int index );

    /** Route state changed */
    void updateRouteState( RoutingManager::State state );

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

    /** Toggle visibility of alternative routes */
    void updateAlternativeRoutes();

    /** Select the first routing profile if none is selected yet */
    void selectFirstProfile();

    /** Show the directions ListView */
    void showDirections();

    /** Update the route request with the given profile */
    void setRoutingProfile( int index );

    /** Change selected routing profile to the one set in the shared request */
    void updateActiveRoutingProfile();

    /** Toggle cloud sync button visibility based on whether route sync is enabled */
    void updateCloudSyncButtons();

    /** Open a remote route and center on it */
    void openCloudRoute( const QString &identifier );

    void updateUploadProgress( qint64 sent, qint64 total );

    void centerOn( const GeoDataCoordinates &coordinates );

    void seekTourToStart();

    void initializeTour();

    void clearTour();

    void toggleRoutePlay();

    void handlePlanetChange();

protected:
    bool eventFilter( QObject *o, QEvent *e ) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    RoutingWidgetPrivate *const d;
};

} // namespace Marble

#endif
