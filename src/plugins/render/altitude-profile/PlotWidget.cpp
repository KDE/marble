//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#include "PlotWidget.h"

#include <QMouseEvent>
#include <QDebug>
#include <QPainter>

#include <KPlotPoint>
#include <KPlotObject>

#include "AltitudeProfile.h"

namespace Marble {

PlotWidget::PlotWidget( AltitudeProfile* parent )
    : KPlotWidget( 0 ), m_altitudeProfile ( parent ), m_highligtedPoint( 0 )
{
    setMouseTracking( true );
}

void PlotWidget::mouseMoveEvent( QMouseEvent* ev )
{
    QWidget::mouseMoveEvent( ev );

    QPoint mousePos = ev->pos() - QPoint( leftPadding(), topPadding() ) - contentsRect().topLeft();
    KPlotPoint *point = 0;
    foreach ( KPlotObject *po, plotObjects() ) {
        foreach ( KPlotPoint *pp, po->points() ) {
            if (!point) point = pp;
            QPoint pointPos = mapToWidget( point->position() ).toPoint();
            QPoint curPointPos = mapToWidget( pp->position() ).toPoint();
            if ( mousePos.x() > curPointPos.x() && curPointPos.x() > pointPos.x() ) {
                point = pp;
            }
        }
    }
    if ( point != m_highligtedPoint ) {
        m_highligtedPoint = point;
        update();
        m_altitudeProfile->forceUpdate();
    }
}

void PlotWidget::clearHighligtedPoint()
{
    m_highligtedPoint = 0;
    update();
    m_altitudeProfile->forceUpdate();
}

void PlotWidget::paintEvent( QPaintEvent* ev )
{
    KPlotWidget::paintEvent( ev );
    if ( m_highligtedPoint ) {
        QPainter p;

        QPoint pointPos = mapToWidget( m_highligtedPoint->position() ).toPoint();

        p.begin( this );
        p.translate( leftPadding(), topPadding() );
        p.setPen( Qt::red );
        p.drawLine( pointPos.x(), 0, pointPos.x(), contentsRect().height() - topPadding() - bottomPadding() );
        p.end();
    }
}


}

#include "PlotWidget.moc"
