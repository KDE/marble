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


#ifndef CONTROLVIEW_H
#define CONTROLVIEW_H


#include <QtGui/QWidget>
#include <QtGui/QPixmap>

#include "MarbleWidget.h"
#include "MarbleControlBox.h"

class QSplitter;



class ControlView : public QWidget
{
    Q_OBJECT

 public:
    ControlView( QWidget * = 0 );
    virtual ~ControlView(){}

    MarbleWidget      *marbleWidget()  const { return m_marbleWidget; }
    MarbleControlBox  *marbleControl() const { return m_control;      }

    void zoomIn();
    void zoomOut();
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

    bool sideBarShown() const { return m_control->isVisible(); }

    void addPlaceMarkFile( QString filename ){ m_marbleWidget->addPlaceMarkFile( filename ); }

    QPixmap mapScreenShot(){ return m_marbleWidget->mapScreenShot(); }
    
    SunLocator* sunLocator() {return m_marbleWidget->sunLocator();}
    

 public slots:
    void setSideBarShown( bool );    
    void setNavigationTabShown( bool );
    void setLegendTabShown( bool );
    void setMapViewTabShown( bool );
    void setCurrentLocationTabShown( bool );
    void setFileViewTabShown( bool );
    void showSunShading(bool show) {m_marbleWidget->showSunShading(show);}
    void centerSun() {m_marbleWidget->centerSun();}

 private:
    //MarbleModel       *m_marbleModel;
    MarbleWidget      *m_marbleWidget;
    MarbleControlBox  *m_control;
    QSplitter         *m_splitter;
};


#endif // CONTROLVIEW_H
