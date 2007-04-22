//
// C++ Interface: vecmapper
//
// Description: VectorComposer 

// The VectorComposer maps the data stored as polylines onto the
// respective projection.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution

#ifndef VECTORCOMPOSER_H
#define VECTORCOMPOSER_H


#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPen>
#include <QtGui/QPixmap>

#include "Quaternion.h"


/**
@author Torsten Rahn
*/


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
