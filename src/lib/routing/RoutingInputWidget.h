//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ROUTINGINPUTWIDGET_H
#define MARBLE_ROUTINGINPUTWIDGET_H

#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include "PositionProviderPluginInterface.h"

#include <QtGui/QWidget>

class QNetworkReply;
class QAbstractItemModel;
class QMenu;

namespace Marble
{

class RoutingInputWidgetPrivate;
class MarblePlacemarkModel;
class RouteRequest;
class PluginManager;
class MarbleWidget;

/**
  * Combines a line edit for input and a couple of buttons to let
  * the user type in a search term, find according placemarks using
  * the marble runner manager and store one of them as the current
  * selection (target position)
  */
class RoutingInputWidget : public QWidget
{
    Q_OBJECT

public:

    /** Constructor */
    explicit RoutingInputWidget( MarbleWidget* widget, int index, QWidget *parent = 0 );

    /** Destructor */
    ~RoutingInputWidget();

    /**
      * Returns true if the user has selected a valid geo position
      * @see targetPosition
      */
    bool hasTargetPosition() const;

    /**
      * Returns the geoposition selected by the user, or a default
      * constructed geoposition if hasTargetPosition returns false
      * @see hasTargetPosition selectPlacemark
      */
    GeoDataCoordinates targetPosition() const;

    /**
      * Returns the placemark model that contains search results
      */
    MarblePlacemarkModel *searchResultModel();

    /**
      * Returns false iff the input text is empty
      */
    bool hasInput() const;

    /**
      * Change the data index in the route request model
      */
    void setIndex( int index );

    /**
      * Remove target position and user input, if any
      */
    void clear();

    /**
      * Set the progress animation to use. Each icon in the vector is displayed
      * sequentially to create the animation, the animation loops endlessly.
      */
    void setProgressAnimation( const QVector<QIcon> &animation );

public Q_SLOTS:
    /**
      * Search for placemarks matching the current input text. Does nothing
      * if input is empty
      */
    void findPlacemarks();

    /** Set the target position to the given coordinates,
      * eliminating any previously set positions
      * @see selectPlacemark hasTargetPosition
      */
    void setTargetPosition( const GeoDataCoordinates &position, const QString &name = QString() );

    /** Cancel a started input request from the map */
    void abortMapInputRequest();

    /** Reload the bookmarks menu */
    void reloadBookmarks();

Q_SIGNALS:
    /** All runners are finished */
    void searchFinished( RoutingInputWidget * );

    /** User requests to remove this widget */
    void removalRequest( RoutingInputWidget * );

    /** User requests to activate this widget */
    void activityRequest( RoutingInputWidget * );

    /** User requests position input from the map */
    void mapInputModeEnabled( RoutingInputWidget *, bool enabled );

    /** hasTargetPosition changed because of selecting a placemark or changing the search term */
    void targetValidityChanged( bool targetValid );

private Q_SLOTS:
    /** Runner progress */
    void setPlacemarkModel( QAbstractItemModel * );

    /** Handle click on the goto target button */
    void requestActivity();

    /** Handle click on the remove widget button */
    void requestRemoval();

    /** Handle click on the map input button */
    void setMapInputModeEnabled( bool enabled );

    /** All runners have completed */
    void finishSearch();

    /** Mark ourself dirty (no target) */
    void setInvalid();

    /** Set the target position (dragging) */
    void updatePosition( int index, const GeoDataCoordinates &position );

    void reverseGeocoding();

    void retrieveReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

    void setHomePosition();

    void setCurrentLocation();

    void updateCurrentLocationButton( PositionProviderStatus status );

    void updateCenterButton( bool hasPosition );

    void setBookmarkPosition( QAction* bookmark );

    void openTargetSelectionDialog();

    void showMenu();

private:
    RoutingInputWidgetPrivate *const d;
};

} // namespace Marble

#endif
