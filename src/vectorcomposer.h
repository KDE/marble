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

    void drawTextureMap(QPaintDevice*, const int&, Quaternion&);
    void paintVectorMap(ClipPainter*, const int&, Quaternion&);
    void resizeMap(const QPaintDevice *);

 private:
    PntMap *pcoast;

    PntMap *pisland;
    PntMap *plake;
    PntMap *pglacier;
    PntMap *priver;

    PntMap *pborder;            // The country borders
    PntMap *pusa;               // The states of the USA

    VectorMap  *vectormap;

    QPen        m_areapen;
    QPen        m_riverpen;
    QPen        m_borderpen;
    QBrush      m_areabrush;
    QBrush      m_lakebrush;

    QVector<qreal> m_dashes;
};

#endif // VECTORCOMPOSER_H
