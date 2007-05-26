//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "katlascrosshair.h"

#include <QtCore/QDebug>


KAtlasCrossHair::KAtlasCrossHair(QObject* parent)
    : QObject(parent)
{
    m_enabled = false;
}


void KAtlasCrossHair::paintCrossHair( QPainter* painter, int width, int height)
{

    if ( m_enabled ) { 
        int  centerx  = width / 2;
        int  centery  = height / 2;
        int  halfsize = 5;

        painter->setPen( QColor( Qt::white ) );
        painter->drawLine( centerx - halfsize, centery,
                           centerx + halfsize, centery );
        painter->drawLine( centerx, centery - halfsize,
                           centerx, centery + halfsize );
    }	
}


#ifndef Q_OS_MACX
#include "katlascrosshair.moc"
#endif
