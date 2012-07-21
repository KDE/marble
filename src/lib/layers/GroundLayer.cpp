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
// Copyright 2010,2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Cezar Mocan <mocancezar@gmail.com>
//

#include "GroundLayer.h"

#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QtCore/QDebug>

namespace Marble
{

QStringList GroundLayer::renderPosition() const
{
    return QStringList() << "SURFACE";
}

bool GroundLayer::render( GeoPainter *painter,
                              ViewportParams *viewParams,
                              const QString &renderPos,
                              GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    // Right now hardcoded, will be taken from the <map bgcolor=""> attribute of the DGML. !!! Have a look at MarbleModel.cpp:274
    QBrush    oceanBrush( QBrush( QColor( 153, 179, 204 ) ) );
    QPen      oceanPen( Qt::NoPen );

    // First I have to check if the map has texture layers; doable by transmitting the mapTheme or the model as a parameter up to here
    // !!! Have a look at LayerManager.cpp:185 and MarbleMap.cpp:698
    painter->setBrush( oceanBrush );
    painter->setPen( oceanPen );
    painter->drawPath( viewParams->mapShape() );

    return true;
}

qreal GroundLayer::zValue() const
{
    return -50.0;
}

}
