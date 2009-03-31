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
#include <QtCore/QPoint>
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QCursor>

class QEvent;
class QTimer;
class QRubberBand;

namespace Marble
{

class MarbleModel;
class MarbleWidget;
class MarbleMap;
class MarbleWidgetPopupMenu;

class MarbleWidgetInputHandler  : public QObject
{
    Q_OBJECT

 public:
    MarbleWidgetInputHandler();
    virtual ~MarbleWidgetInputHandler();

    void setPositionSignalConnected( bool connected ){ m_positionSignalConnected = connected; }
    bool isPositionSignalConnected() const { return m_positionSignalConnected; }

    virtual void init(MarbleWidget*);

 Q_SIGNALS:
    // Mouse button menus
    void lmbRequest( int, int );
    void rmbRequest( int, int );

    //Gps coordinates
    void mouseClickScreenPosition( int, int );
    void mouseMoveGeoPosition( QString );

 protected Q_SLOTS:
    void restoreViewContext();

 protected:
    Q_DISABLE_COPY( MarbleWidgetInputHandler )
    MarbleWidget  *m_widget;
    MarbleModel   *m_model;

    bool     m_positionSignalConnected;

    QTimer   *m_mouseWheelTimer;
};



class MarbleWidgetDefaultInputHandler  : public MarbleWidgetInputHandler
{
 Q_OBJECT

 public:
    MarbleWidgetDefaultInputHandler();
    ~MarbleWidgetDefaultInputHandler();

    static bool keyEvent( MarbleMap * map, QEvent* e );
    
    virtual void init(MarbleWidget*);
    
    /**
     * @brief  Set whether a popup menu appears on a click (not drag) with the left mouse button
     * @param  enabled True to enable the popup menu (default), false to disable it
     */
    void setLeftMouseButtonPopup(bool enabled);
    
    /**
     * @brief  Return whether the left mouse button popup menu is active
     * @return True iff a popup menu is shown on left mouse button clicks
     */
    bool leftMouseButtonPopup();

 protected:
    bool eventFilter( QObject *, QEvent * );

 private Q_SLOTS:
    void showLmbMenu( int, int );
    
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

    int      m_dirX;
    int      m_dirY;

    bool     m_leftpressed;
    bool     m_midpressed;
    int      m_leftpressedx;
    int      m_leftpressedy;
    int      m_midpressedy;
    qreal   m_leftpresseda;
    qreal   m_leftpressedb;

    int      m_dragThreshold;
    QTime    m_dragtimer;

    QPoint       m_selectionOrigin;
    QRubberBand *m_selectionRubber;
    
    MarbleWidgetPopupMenu     *m_popupmenu;
    
    bool m_leftpopup;
};

}

#endif // MARBLEWIDGETINPUTHANDLER
