//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//
#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H


#include <QtCore/QVector>
#include <QtCore/QPointF>
#include <QtCore/QString>

// NOTE: THIS CLASS IS DEPRECATED. Use GeoDataLatLonBox or GeoDataLatLonAltBox instead!

class BoundingBox
{
 public:
    BoundingBox();
    explicit BoundingBox( const QVector<QPointF> & );

    bool     contains( const QPointF & ) const;
    bool     intersects( const BoundingBox & ) const;
    bool     isValid() const;
    QString  string() const;

 private:
    void  init();

 private:
    double m_topX;
    double m_bottomX;
    double m_topY;
    double m_bottomY;

    bool m_dateLine;
    // bool m_valid;
};

#endif //BOUNDINGBOX_H
