//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2011,2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "FpsLayer.h"

#include <QPoint>
#include <QTime>
#include <QFont>
#include <QPainter>

namespace Marble
{

FpsLayer::FpsLayer( QTime *time )
    : m_time( time )
{
}

void FpsLayer::paint( QPainter *painter ) const
{
    const qreal fps = 1000.0 / (qreal)( m_time->elapsed() );
    const QString fpsString = QString("Speed: %1 fps").arg(fps, 5, 'f', 1, QLatin1Char(' '));

    const QPoint fpsLabelPos( 10, 20 );

    painter->setFont( QFont( QStringLiteral( "Sans Serif" ), 10 ) );

    painter->setPen( Qt::black );
    painter->setBrush( Qt::black );
    painter->drawText( fpsLabelPos, fpsString );

    painter->setPen( Qt::white );
    painter->setBrush( Qt::white );
    painter->drawText( fpsLabelPos.x() - 1, fpsLabelPos.y() - 1, fpsString );
}

}
