//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_FPSLAYER_H
#define MARBLE_FPSLAYER_H

#include "LayerInterface.h"

class QTime;

namespace Marble
{

class FpsLayer : public LayerInterface
{
public:
    FpsLayer( QTime *time );

    virtual QStringList renderPosition() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

private:
    QTime *const m_time;
};

}

#endif
