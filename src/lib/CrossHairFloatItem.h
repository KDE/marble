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

//
// KAtlasCrossHair paints a crosshair
//


#ifndef CROSSHAIRFLOATITEM_H
#define CROSSHAIRFLOATITEM_H


#include <QtCore/QObject>

class QPainter;

class CrossHairFloatItem : public QObject
{
    Q_OBJECT

 public:
    explicit CrossHairFloatItem(QObject *parent = 0);

    void paint( QPainter*, int, int );

    const bool enabled()            const { return m_enabled; }
    void       setEnabled( bool enabled ) { m_enabled = enabled; }

 protected:
    bool  m_enabled;
};


#endif // CROSSHAIRFLOATITEM_H
