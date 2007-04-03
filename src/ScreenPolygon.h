#ifndef SCREENPOLYGON_H
#define SCREENPOLYGON_H


#include <QtCore/QVector>
#include <QtGui/QPolygon>


class ScreenPolygon : public QPolygonF 
{
 public:
    ScreenPolygon() : m_closed( false ) { }
    ScreenPolygon( bool closed ) : m_closed( closed ) { }
    ~ScreenPolygon() { }

    bool closed() const { return m_closed; }
    void setClosed( bool closed ) { m_closed = closed; }

    // Type definitions
    typedef QVector<ScreenPolygon> Vector;

 protected:
    // true if the polygon is closed
    bool  m_closed;
};

#endif // SCREENPOLYGON_H
