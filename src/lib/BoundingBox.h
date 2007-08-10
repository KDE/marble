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

class BoundingBox : public QVector<QPointF>
{
 public:
    BoundingBox();
    BoundingBox( const QVector<QPointF> & );
    bool contains( const QPointF & );
    bool intersects( BoundingBox );
    bool isValid();
    QString string();
 private:
    bool m_dateLine;
    bool m_valid;
    double m_topX;
    double m_botX;
    double m_topY;
    double m_botY;
};

#endif //BOUNDINGBOX_H
