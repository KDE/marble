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



#include <QtGui/QWidget>
#include <QtGui/QPixmap>

#include "lib/MarbleWidget.h"


class QSplitter;
class MarbleWidget;
//class MarbleModel;
class MarbleControlBox;


class KAtlasControl : public QWidget
{
    Q_OBJECT

 public:
    KAtlasControl(QWidget *);
    virtual ~KAtlasControl(){}

    MarbleWidget  *marbleWidget() const { return m_marbleWidget; }

    void zoomIn();
    void zoomOut();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();


    void addPlaceMarkFile( QString filename ){ m_marbleWidget->addPlaceMarkFile( filename ); }

    QPixmap mapScreenShot(){ return m_marbleWidget->mapScreenShot(); }

 public slots:
    void setSideBarShown( bool );    
    void setNavigationTabShown( bool );
    void setLegendTabShown( bool );
    void setMapViewTabShown( bool );
    void setCurrentLocationTabShown( bool );

 private:
    //MarbleModel       *m_marbleModel;
    MarbleWidget      *m_marbleWidget;
    MarbleControlBox  *m_control;
    QSplitter         *m_splitter;
};


#endif // KATLASCONTROL_H
