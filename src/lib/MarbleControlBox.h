//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
//

#ifndef MARBLE_MARBLECONTROLBOX_H
#define MARBLE_MARBLECONTROLBOX_H


#include "marble_export.h"

#include <QtGui/QToolBox>

#include "global.h"


/** @file
 * This file contains the header for MarbleControlBox.
 *
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */

class QModelIndex;
class QResizeEvent;
class QStandardItemModel;
class QString;

namespace Marble
{

class MarbleWidget;
class MarbleControlBoxPrivate;
class MarblePlacemarkModel;
class AdjustNavigation;
/**
 * @short A widget class that contains advanced controls for a
 * MarbleWidget.
 *
 * This widget lets the user control an instance of MarbleWidget.  The
 * widget contains a ToolBox with 3 pages: a <b>Navigation</b> page
 * with a search tool for cities and other names, a <b>Legend</b> page
 * with a legend for the symbols on the map, and a <b>Map View</b>
 * page with a choice of themes / datasets.
 *
 * The <b>Navigation</b> page lets the user navigate around the globe by
 * using buttons for panning and a slider for zooming.  There is also
 * a "Home" button that lets the user go back to a predefined point at
 * the earth, not unlike the home page in a web browser.  In addition
 * there is a search tool that lets the user search for names of
 * points of interest (cities, mountains, glaciers, etc).
 *
 * The <b>Legend</b> page does not contain any controls, but displays
 * a legend of the symbols that are displayed on the globe.
 *
 * The <b>Map View</b> page gives the user a choice of different
 * datasets to display, which can also be thought of as different
 * visual themes. The default datasets are a standard Atlas view, a
 * Night view, and a Satellite view.
 *
 * @see MarbleWidget
 * @see MarbleNavigator
 */

class MARBLE_EXPORT MarbleControlBox : public QToolBox
{
    Q_OBJECT

 public:
    /**
     * @brief Construct a new MarbleControlBox
     * @param parent the parent widget
     */
    explicit MarbleControlBox( QWidget *parent = 0 );
    ~MarbleControlBox();

    /**
     * @brief Add a MarbleWidget to be controlled by this widget.
     * @param widget  the MarbleWidget to be added.
     *
     *  Add a MarbleWidget so that it will be controlled by this
     *  widget.  This call create all the connections of signals and
     *  slots that are necessary to have full control of the
     *  MarbleWidget, and thereby frees the programmer from doing it.
     */
    void addMarbleWidget( MarbleWidget *widget );

    /**
     * @brief Return the minimum zoom level set in the widget.
     * @return the minimum zoom level set in the widget.
     */
    int minimumZoom() const;

    void setMapThemeModel( QStandardItemModel *mapThemeModel );

    void updateCelestialModel();

    /**
      * Toggle offline mode of download manager and runners.
      */
    void setWorkOffline(bool offline);

 Q_SIGNALS:
    /**
     * @brief Signal emitted when the Home button has been pressed.
     */
    void goHome();
    /**
     * @brief Signal emitted when the Zoom In button has been pressed.
     */
    void zoomIn();
    /**
     * @brief Signal emitted when the Zoom Out button has been pressed.
     */
    void zoomOut();
    /**
     * @brief Signal emitted when the zoom slider has been moved.
     * @param zoom  The new zoom value.
     */
    void zoomChanged( int zoom );

    /**
     * @brief Signal emitted when the Move Left button has been pressed.
     */
    void moveLeft();
    /**
     * @brief Signal emitted when the Move Right button has been pressed.
     */
    void moveRight();
    /**
     * @brief Signal emitted when the Move Up button has been pressed.
     */
    void moveUp();
    /**
     * @brief Signal emitted when the Move Down button has been pressed.
     */
    void moveDown();
    /**
     * @brief Signal emitted when a user selects a placemark in the search widget.
     * @param index  the index for the chosen placemark.
     *
     * This signal is emitted when the user has selected a placemark
     * in the search, e.g. by double clicking it or by pressing
     * return.  If it is connected to the centerOn( QModelIndex&) slot
     * in a MarbleWidget, the widget will center the view on this
     * placemark.
     */
    void centerOn( const QModelIndex&, bool );

    void selectMapTheme( const QString& );

    void projectionSelected( Projection );

 public Q_SLOTS:

    void selectTheme( const QString & );

    void selectCurrentMapTheme( const QString& );

    void selectProjection( Projection projection );

    /**
     * @brief Set a list/set of placemark names for the search widget.
     * @param locations  the QAbstractitemModel containing the placemarks
     *
     * This function is called to display a potentially large number
     * of placemark names in a widget and let the user search between
     * them.
     * @see centerOn
     */
    void setLocations( MarblePlacemarkModel* locations );

    /**
     * @brief Sets the value of the slider.
     * @param zoom The new zoom value.
     *
     * This slot should be called when the zoom value is changed from
     * the widget itself, e.g. by using the scroll wheel.  It sets the
     * value of the slider, but nothing more.  In particular it
     * doesn't emit the zoomChanged signal.
     */
    void changeZoom( int zoom );
    void enableFileViewActions();

    /**
     * @brief Control whether the Navigation tab is shown.
     * @param show  boolean that controls if the Navigation tab is shown.
     */
    void setNavigationTabShown( bool show );
    /**
     * @brief Control whether the Legend tab is shown.
     * @param show  boolean that controls if the Legend tab is shown.
     */
    void setLegendTabShown( bool show );
    /**
     * @brief Control whether the Map View tab is shown.
     * @param show  boolean that controls if the Map View tab is shown.
     */
    void setMapViewTabShown( bool show );
    /**
     * @brief Control whether the Current Location tab is shown.
     * @param show  boolean that controls if the Current Location tab is shown.
     */
    void setCurrentLocationTabShown( bool show );
    /**
     * @brief Control whether the File View tab is shown.
     * @param show  boolean that controls if the File View tab is shown.
     */
    void setFileViewTabShown( bool show );

    /// whenever a new map gets inserted, the following slot will adapt the ListView accordingly
    void updateMapThemeView();

    void projectionSelected( int projectionIndex );

 private Q_SLOTS:

    /// called whenever the user types something new in the search box
    void searchLineChanged( const QString &search );

    /// called the Return or Enter key is pressed in the search box.
    void searchReturnPressed();

    /// called by the singleShot to initiate a search based on the searchLine
    void search();

    void updateButtons( int );
    void mapCenterOnSignal( const QModelIndex & );

    void adjustForAnimation();
    void adjustForStill();

 private:
    void setWidgetTabShown( QWidget * widget, int insertIndex,
                            bool show, QString &text );

protected:
    /**
     * @brief Reimplementation of the resizeEvent() of the widget.
     *
     * If the MarbleControlBox gets shrunk enough, the slider in the
     * Navigation tab will be hidden, leaving only the Zoom Up and
     * Zoom Down buttons.
     */
    void resizeEvent( QResizeEvent * );

 private:
    Q_DISABLE_COPY( MarbleControlBox )
    MarbleControlBoxPrivate * const d;
};

}

#endif
