//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin   <ingwa@kde.org>"
//

//
// This widget is a control box for the Marble widget.
//


#ifndef MARBLENAVIGATOR_H
#define MARBLENAVIGATOR_H


#include <QtGui/QWidget>

#include "marble_export.h"


/** @file
 * This file contains the header for MarbleNavigator
 * 
 * @author Torsten Rahn <tackat@kde.org>
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


class MarbleNavigatorPrivate;

/** 
 * @short A widget class that contains simple controls for a
 * MarbleWidget.
 *
 * This widget lets the user control an instance of MarbleWidget.  The
 * user can control panning and zooming as well as returning to a
 * predefined view called the 'home' position.
 *
 * @see MarbleWidget
 * @see MarbleControlBox
 */

class MARBLE_EXPORT MarbleNavigator : public QWidget
{
    Q_OBJECT

 public:
    MarbleNavigator(QWidget *parent = 0);
 
    int  minimumZoom() const;
	
 Q_SIGNALS:
    void goHome();
    void zoomIn();
    void zoomOut();
    void zoomChanged(int);

    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    //void centerOn(const QModelIndex&);

 public Q_SLOTS:
    void changeZoom(int);

 protected:
    void resizeEvent ( QResizeEvent * );

 private:
    MarbleNavigatorPrivate  * const d;
};

#endif // MARBLENAVIGATOR_H
