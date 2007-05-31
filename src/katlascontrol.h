//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// Testapplication with controls
//


#ifndef KATLASCONTROL_H
#define KATLASCONTROL_H


#include "MarbleWidget.h"

#include <QtGui/QPixmap>

/**
@author Torsten Rahn
*/

class MarbleControlBox;

class KAtlasControl : public QWidget
{
    Q_OBJECT

 public:
    KAtlasControl(QWidget *);
    virtual ~KAtlasControl(){};

    void zoomIn();
    void zoomOut();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    void addPlaceMarkFile( QString filename ){ m_marbleWidget->addPlaceMarkFile( filename ); }

    QPixmap mapScreenShot(){ return m_marbleWidget->mapScreenShot(); }

 private:
    KAtlasGlobe       *m_globe;
    MarbleWidget      *m_marbleWidget;
    MarbleControlBox  *m_control;
};


#endif // KATLASCONTROL_H
