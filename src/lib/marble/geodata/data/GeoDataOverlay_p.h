//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_GEODATAOVERLAY_P_H
#define MARBLE_GEODATAOVERLAY_P_H

#include "GeoDataFeature_p.h"

#include <QImage>
#include <QColor>

namespace Marble {

class GeoDataOverlayPrivate : public GeoDataFeaturePrivate
{
public:
    QColor m_color;

    int m_drawOrder;

    mutable QImage m_image;

    QString m_iconPath;

    GeoDataOverlayPrivate()
        : m_color(Qt::white),
          m_drawOrder(0)
    {
    }

    GeoDataOverlayPrivate(const GeoDataOverlayPrivate& other)
      : GeoDataFeaturePrivate(other),
        m_color(Qt::white),
        m_drawOrder(0)
    {
    }
};

}

#endif
