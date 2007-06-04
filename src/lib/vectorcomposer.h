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
// The VectorComposer maps the data stored as polylines onto the
// respective projection.
//
// Author: Torsten Rahn
//


#ifndef VECTORCOMPOSER_H
#define VECTORCOMPOSER_H


#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPen>
#include <QtGui/QPixmap>

#include "Quaternion.h"


class ClipPainter;
class PntMap;
class VectorMap;


class VectorComposer
{
 public:
    VectorComposer();
    virtual ~VectorComposer(){};

    void  drawTextureMap(QPaintDevice*, const int&, Quaternion&);
    void  paintVectorMap(ClipPainter*, const int&, Quaternion&);
    void  resizeMap(const QPaintDevice *);

 private:
    VectorMap  *m_vectorMap;

    PntMap     *m_coastLines;

    PntMap     *m_islands;
    PntMap     *m_lakes;
    PntMap     *m_glaciers;
    PntMap     *m_rivers;

    PntMap     *m_countries;    // The country borders
    PntMap     *m_usaStates;    // The states of the USA

    QPen        m_areapen;
    QBrush      m_areabrush;
    QPen        m_riverpen;
    QPen        m_borderpen;
    QBrush      m_lakebrush;

    QVector<qreal> m_dashes;
};

#endif // VECTORCOMPOSER_H
