//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>"
// Copyright 2007      Inge Wallin   <ingwa@kde.org>"
// Copyright 2007      Thomas Zander <zander@kde.org>"
//

//
// The TextureColorizer maps the Elevationvalues to Legend Colors.
//


#ifndef MARBLECONTROLBOX_H
#define MARBLECONTROLBOX_H


#include "ui_MarbleControlBox.h"


/** @file
 * This file contains the header for MarbleControlBox.
 * 
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


class MarbleWidget;
class QStringListModel;

/** 
 * @short A widget class that contains all sorts of controls for a
 * MarbleWidget.
 *
 * This widget lets the user control an instance of MarbleWidget.  The
 * widget contains a ToolBox with 3 pages: a <b>Navigation</b> page
 * with a search tool for cities and other names, a <b>Legend</b> page
 * with a legend for the symbols on the map, and a <b>Map View</b>
 * page with a choice of themes / datasets.
 *
 * The <b>Navigation</b> pane lets the user navigate around the globe by
 * using buttons for panning and a slider for zooming.  There is also
 * a "home" button that lets the user go back to a predefined point at
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
 * Night view, and a Sattelite view.
 *
 * @see MarbleWidget
 */

class MarbleControlBox : public QWidget, private Ui::MarbleControlBox
{
    Q_OBJECT

 public:
    MarbleControlBox(QWidget *parent = 0);
 
    void addMarbleWidget( MarbleWidget *widget );

    void setLocations(QAbstractItemModel* locations) {
        locationListView->setModel( locations );
    }

    int minimumZoom() const { return m_minimumzoom; }
	
 signals:
    void goHome();
    void zoomIn();
    void zoomOut();
    void zoomChanged(int);

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void centerOn(const QModelIndex&);

    void selectMapTheme( const QString& );

 public slots:
    void changeZoom(int);

 private slots:
    /// called whenever the user types something new in the search box
    void searchLineChanged(const QString &search);

    /// called by the singleShot to initiate a search based on the searchLine
    void search();

 protected:
    void resizeEvent ( QResizeEvent * );

 private:
    MarbleWidget  *m_widget;
    QString        m_searchTerm;
    bool           m_searchTriggered;
    int            m_minimumzoom;
};

#endif // MARBLECONTROLBOX_H
