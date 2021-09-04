//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2009 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2011, 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "FpsLayer.h"

#include <QPoint>
#include <QElapsedTimer>
#include <QFont>
#include <QPainter>

namespace Marble
{

FpsLayer::FpsLayer( QElapsedTimer *time )
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
