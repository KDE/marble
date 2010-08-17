//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010,2011  Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_MARBLEGLVIEW_H
#define MARBLE_MARBLEGLVIEW_H


#include <QtOpenGL/QGLWidget>


namespace Marble
{

class MarbleMap;

/**
 * @short A widget class that displays a view of the earth.
 *
 * This widget displays a view of the earth or any other globe,
 * depending on which dataset is used.  The user can navigate the
 * globe using either a control widget, e.g. the MarbleControlBox, or
 * the mouse.  The mouse and keyboard control is done through a
 * MarbleWidgetInputHandler. Only some aspects of the widget can be
 * controlled by the mouse and/or keyboard.
 *
 * By clicking on the globe and moving the mouse, the position can be
 * changed.  The user can also zoom by using the scroll wheel of the
 * mouse in the widget. The zoom value is not tied to any units, but
 * is an abstract value without any physical meaning. A value around
 * 1000 shows the full globe in a normal-sized window. Higher zoom
 * values give a more zoomed-in view.
 *
 * The MarbleWidget needs to be provided with a data model to
 * work. This model is contained in the MarbleModel class, and it is
 * painted by using a MarbleMap. The widget can also construct its own
 * map and model if none is given to the constructor.  A MarbleModel
 * contains 3 separate datatypes: <b>tiles</b> which provide the
 * background, <b>vectors</b> which provide things like country
 * borders and coastlines and <b>placemarks</b> which can show points
 * of interest, such as cities, mountain tops or the poles.
 *
 * In addition to navigating with the mouse, you can also use it to
 * get information about items on the map. You can either click on a
 * placemark with the left mouse button or with the right mouse button
 * anywhere on the map.
 *
 * The left mouse button opens up a menu with all the placemarks
 * within a certain distance from the mouse pointer. When you choose
 * one item from the menu, Marble will open up a dialog window with
 * some information about the placemark and also try to connect to
 * Wikipedia to retrieve an article about it. If there is such an
 * article, you will get a mini-browser window with the article in a tab.
 *
 * The right mouse button controls a distance tool.  The distance tool
 * is implemented as a menu where you can choose to either create or
 * remove so called Measure Points. Marble will keep track of the
 * Measure Points and show the total distance in the upper left of the
 * widget.  Measure Points are shown on the map as a little white
 * cross.
 *
 * @see MarbleMap
 */

class MarbleGLView : public QGLWidget
{
    Q_OBJECT

 public:

    /**
     * @brief Construct a new MarbleGLView.
     * @param map  the data map for the widget.
     * @param parent the parent widget
     */
    explicit MarbleGLView( MarbleMap *map, QWidget *parent = 0 );

    virtual ~MarbleGLView();

 protected:
    virtual void initializeGL();

    /**
     * @brief Reimplementation of the paintEvent() function in QWidget.
     */
    virtual void paintEvent( QPaintEvent *event );

    /**
     * @brief Reimplementation of the resizeGLt() function in QGLWidget.
     */
    virtual void resizeGL( int width, int height );

 private:
    Q_DISABLE_COPY( MarbleGLView )
    class Private;
    Private  * const d;
};

}

#endif
