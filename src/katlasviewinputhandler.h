//
// C++ Interface: katlasviewinputhandler.h
//
// Description: KAtlasViewInputHandler 

// The KAtlasViewInputHandler handles mouse and keyboard input.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution


#ifndef KATLASVIEWINPUTHANDLER_H
#define KATLASVIEWINPUTHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QTime>
#include <QtGui/QKeyEvent>
#include <QtGui/QPixmap>
#include <QtGui/QCursor>

#include "katlasglobe.h"


/**
@author Torsten Rahn
*/
class KAtlasView;

class KAtlasViewInputHandler  : public QObject
{
    Q_OBJECT

 public:
    KAtlasViewInputHandler(KAtlasView*, KAtlasGlobe*);
    virtual ~KAtlasViewInputHandler(){};

 signals:
    // Mouse button menus
    void lmbRequest( int, int );
    void rmbRequest( int, int );

 protected:
    bool eventFilter( QObject *, QEvent * );

 private:
    KAtlasView   *m_gpview;
    KAtlasGlobe  *m_globe;

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
    float    m_leftpresseda;
    float    m_leftpressedb;

    int      m_dragThreshold;
    QTime    m_dragtimer;
};


#endif // KATLASVIEWINPUTHANDLER
