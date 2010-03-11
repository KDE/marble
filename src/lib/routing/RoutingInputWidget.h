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

#ifndef MARBLE_ROUTING_TARGET_INPUT_WIDGET_H
#define MARBLE_ROUTING_TARGET_INPUT_WIDGET_H

#include "GeoDataCoordinates.h"

#include <QtGui/QWidget>

namespace Marble {

class RoutingInputWidgetPrivate;
class MarblePlacemarkModel;

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
    explicit RoutingInputWidget(QWidget *parent = 0);

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
    MarblePlacemarkModel* searchResultModel();

    /**
      * @brief Hide all buttons iff simple is true
      */
    void setSimple(bool simple);

    /**
      * Returns false iff the input text is empty
      */
    bool hasInput() const;

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
    void setTargetPosition(const GeoDataCoordinates &position);

Q_SIGNALS:
    /** All runners are finished */
    void searchFinished(RoutingInputWidget*);

    /** User requests to remove this widget */
    void removalRequest(RoutingInputWidget*);

    /** User requests to activate this widget */
    void activityRequest(RoutingInputWidget*);

    /** User requests position input from the map */
    void mapInputRequest(RoutingInputWidget*);

    /** hasTargetPosition changed because of selecting a placemark or changing the search term */
    void targetValidityChanged(bool targetValid);

private Q_SLOTS:
    /** Runner progress */
    void setPlacemarkModel(MarblePlacemarkModel*);

    /** Handle click on the goto target button */
    void requestActivity();

    /** Handle click on the remove widget button */
    void requestRemoval();

    /** Handle click on the map input button */
    void requestMapPosition();

    /** Progress animation update */
    void updateProgress();

    /** All runners have completed */
    void finishSearch();

    /** Mark ourself dirty (no target) */
    void setInvalid();

private:
    RoutingInputWidgetPrivate* const d;
};

} // namespace Marble

#endif // MARBLE_ROUTING_TARGET_INPUT_WIDGET_H
