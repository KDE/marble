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

class QResizeEvent;
class QString;
class QStandardItemModel;

namespace Marble
{

class MarbleWidget;
class MarbleControlBoxPrivate;
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
      * Toggle offline mode of download manager and runners.
      */
    void setWorkOffline(bool offline);

    void setMapThemeModel( QStandardItemModel *mapThemeModel );

 public Q_SLOTS:
    void selectTheme( const QString & );

    void updateMapThemeView();

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

    /**
     * @brief Control whether the Routing tab is shown.
     * @param show  boolean that controls if the Routing tab is shown.
     */
    void setRoutingTabShown( bool show );

 private:
    void setWidgetTabShown( QWidget * widget, int insertIndex,
                            bool show, QString &text );

    Q_DISABLE_COPY( MarbleControlBox )
    MarbleControlBoxPrivate * const d;
};

}

#endif
