//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_SCREENPOLYGON_H
#define MARBLE_SCREENPOLYGON_H


#include <QtCore/QVector>
#include <QtGui/QPolygonF>

namespace Marble
{

class ScreenPolygon : public QPolygonF 
{
 public:
    ScreenPolygon() : m_closed( false ) { }
    explicit ScreenPolygon( bool closed ) : m_closed( closed ) { }
    ~ScreenPolygon() { }

    bool closed() const { return m_closed; }
    void setClosed( bool closed ) { m_closed = closed; }

    // Type definitions
    typedef QVector<ScreenPolygon> Vector;

 protected:
    // true if the polygon is closed
    bool  m_closed;
};

}

#endif
