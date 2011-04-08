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

#include "FpsLayer.h"

#include <QtCore/QTime>

#include "GeoPainter.h"

namespace Marble
{

FpsLayer::FpsLayer( QTime *time )
    : m_time( time )
{
}

QStringList FpsLayer::renderPosition() const
{
    return QStringList() << "BEHIND_TARGET";
}

bool FpsLayer::render( GeoPainter *painter,
                       ViewportParams *viewParams,
                       const QString &renderPos,
                       GeoSceneLayer *layer )
{
    Q_UNUSED( viewParams )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    qreal fps = 1000.0 / (qreal)( m_time->elapsed() + 1 );

    QString fpsString = QString( "Speed: %1 fps" ).arg( fps, 5, 'f', 1, QChar(' ') );

    QPoint fpsLabelPos( 10, 20 );

    painter->setFont( QFont( "Sans Serif", 10 ) );

    painter->setPen( Qt::black );
    painter->setBrush( Qt::black );
    painter->drawText( fpsLabelPos, fpsString );

    painter->setPen( Qt::white );
    painter->setBrush( Qt::white );
    painter->drawText( fpsLabelPos.x() - 1, fpsLabelPos.y() - 1, fpsString );

    return true;
}

}
