//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "FpsLayer.h"

#include <QtCore/QPoint>
#include <QtCore/QTime>
#include <QtGui/QFont>

#include <limits>

#include <GeoPainter.h>

namespace Marble
{

FpsLayer::FpsLayer( QTime *time )
    : m_time( time )
{
}

QStringList FpsLayer::renderPosition() const
{
    return QStringList() << "USER_TOOLS";
}

bool FpsLayer::render( GeoPainter *painter,
                       ViewportParams *viewport,
                       const QString &renderPos,
                       GeoSceneLayer *layer )
{
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    const qreal fps = 1000.0 / (qreal)( m_time->elapsed() );
    const QString fpsString = QString( "Speed: %1 fps" ).arg( fps, 5, 'f', 1, QChar(' ') );

    const QPoint fpsLabelPos( 10, 20 );

    painter->setFont( QFont( "Sans Serif", 10 ) );

    painter->setPen( Qt::black );
    painter->setBrush( Qt::black );
    painter->drawText( fpsLabelPos, fpsString );

    painter->setPen( Qt::white );
    painter->setBrush( Qt::white );
    painter->drawText( fpsLabelPos.x() - 1, fpsLabelPos.y() - 1, fpsString );

    return true;
}

qreal FpsLayer::zValue() const
{
    return std::numeric_limits<qreal>::max();
}

}
