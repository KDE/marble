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
// The MarbleWidgetInputHandler handles mouse and keyboard input.
//


#ifndef MARBLEWIDGETINPUTHANDLER_H
#define MARBLEWIDGETINPUTHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QTime>
#include <QtGui/QKeyEvent>
#include <QtGui/QPixmap>
#include <QtGui/QCursor>

class MarbleModel;
class MarbleWidget;


class MarbleWidgetInputHandler  : public QObject
{
    Q_OBJECT

 public:
    MarbleWidgetInputHandler();
    virtual ~MarbleWidgetInputHandler(){}

    void setPositionSignalConnected( bool connected ){ m_positionSignalConnected = connected; }
    bool isPositionSignalConnected() const { return m_positionSignalConnected; }

    void init(MarbleWidget*);

 Q_SIGNALS:
    // Mouse button menus
    void lmbRequest( int, int );
    void rmbRequest( int, int );

    //Gps coordinates
    void mouseClickScreenPosition( int, int );
    void mouseMoveGeoPosition( QString );

 protected:
    MarbleWidget  *m_widget;
    MarbleModel   *m_model;

    bool     m_positionSignalConnected;
};



class MarbleWidgetDefaultInputHandler  : public MarbleWidgetInputHandler
{
 public:
    MarbleWidgetDefaultInputHandler();

 protected:
    bool eventFilter( QObject *, QEvent * );

 private:
    QPixmap  curpmtl;
    QPixmap  curpmtc;
    QPixmap  curpmtr;
    QPixmap  curpmcr;
    QPixmap  curpmcl;
    QPixmap  curpmbl;
    QPixmap  curpmbc;
    QPixmap  curpmbr;

    QCursor  arrowcur[3][3];

    int      dirx;
    int      diry;

    bool     m_leftpressed;
    bool     m_midpressed;
    int      m_leftpressedx;
    int      m_leftpressedy;
    int      m_midpressedy;
    double   m_leftpresseda;
    double   m_leftpressedb;

    int      m_dragThreshold;
    QTime    m_dragtimer;
};



#endif // MARBLEWIDGETINPUTHANDLER
