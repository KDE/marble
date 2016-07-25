//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
//

//
// This widget is a control box for the Marble widget.
//

#ifndef MARBLE_MARBLENAVIGATOR_H
#define MARBLE_MARBLENAVIGATOR_H


#include <QWidget>

#include "marble_export.h"


/** @file
 * This file contains the header for MarbleNavigator
 * 
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */

namespace Marble
{

class MarbleNavigatorPrivate;

/** 
 * @short A widget class that contains simple controls for a
 * MarbleWidget.
 *
 * This widget lets the user control an instance of MarbleWidget.  The
 * user can control panning and zooming as well as returning to a
 * predefined view called the 'Home' position.  You cannot change the
 * Home position from the MarbleNavigator, though.
 *
 * @see MarbleWidget
 */

class MARBLE_EXPORT MarbleNavigator : public QWidget
{
    Q_OBJECT

 public:
    /**
     * @brief Construct a new MarbleNavigator
     * @param parent the parent widget
     */
    explicit MarbleNavigator(QWidget *parent = 0);
    ~MarbleNavigator();
 
    /**
     * @brief Return the minimum zoom level set in the widget.
     * @return the minimum zoom level set in the widget.
     */
    int  minimumZoom() const;
	
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
    void zoomChanged(int zoom);

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
    //void centerOn(const QModelIndex&);

 public Q_SLOTS:
    /**
     * @brief Sets the value of the slider.
     * @param zoom The new zoom value.
     *
     * This slot should be called when the zoom value is changed from
     * the widget itself, e.g. by using the scroll wheel.  It sets the
     * value of the slider, but nothing more.  In particular it
     * doesn't emit the zoomChanged signal.
     */
    void changeZoom(int zoom);

 protected:
    /**
     * @brief Reimplementation of the resizeEvent() of the widget.  
     *
     * If the MarbleNavigator gets shrunk enough, the slider will be
     * hidden, leaving only the Zoom Up and Zoom Down buttons.
     */
    void resizeEvent ( QResizeEvent * );

 private:
    Q_DISABLE_COPY( MarbleNavigator )
    MarbleNavigatorPrivate  * const d;
};

}

#endif
