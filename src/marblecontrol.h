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


#ifndef MARBLECONTROL_H
#define MARBLECONTROL_H


#include "ui_marblecontrol.h"

/**
@author Inge Wallin
*/

class QStringListModel;

class MarbleControl : public QWidget, private Ui::marbleControl
{
    Q_OBJECT

 public:
    MarbleControl(QWidget *parent = 0);
 
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
    //void centerOn(const QModelIndex&);

 public slots:
    void changeZoom(int);

 protected:
    void resizeEvent ( QResizeEvent * );

 private:
    int  m_minimumzoom;
};

#endif // MARBLECONTROL_H
