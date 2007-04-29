//
// C++ Interface: katlascrosshair
//
// Description: KAtlasCrossHair 

// KAtlasCrossHair paints a crosshair
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution


#ifndef KATLASCROSSHAIR_H
#define KATLASCROSSHAIR_H


#include <QtCore/QObject>
#include <QtGui/QPainter>


/**
@author Torsten Rahn
*/

class KAtlasCrossHair : public QObject
{
    Q_OBJECT

 public:
    KAtlasCrossHair(QObject *parent = 0);

    void paintCrossHair( QPainter*, int, int );

    const bool enabled()            const { return m_enabled; }
    void       setEnabled( bool enabled ) { m_enabled = enabled; }

 protected:
    bool  m_enabled;
};


#endif // KATLASCROSSHAIR_H
