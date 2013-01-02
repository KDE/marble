//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "ArrowDiscWidget.h"

#include "MarbleWidget.h"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmapCache>
#include <QtGui/QPainterPath>

#include <QDebug>

const int min_radius = 5;
const int max_radius = 28;

namespace Marble
{

ArrowDiscWidget::ArrowDiscWidget( QWidget *parent ) :
    QWidget( parent ),
    m_parentMarbleWidget( 0 ),
    m_imagePath( "marble/navigation/navigational_arrows" )
{
    setMouseTracking( true );
}

ArrowDiscWidget::~ArrowDiscWidget()
{
    QPixmapCache::remove( "marble/navigation/navigational_arrows" );
    QPixmapCache::remove( "marble/navigation/navigational_arrows_hover_bottom" );
    QPixmapCache::remove( "marble/navigation/navigational_arrows_hover_left" );
    QPixmapCache::remove( "marble/navigation/navigational_arrows_hover_right" );
    QPixmapCache::remove( "marble/navigation/navigational_arrows_hover_top" );
    QPixmapCache::remove( "marble/navigation/navigational_arrows_press_bottom" );
    QPixmapCache::remove( "marble/navigation/navigational_arrows_press_left" );
    QPixmapCache::remove( "marble/navigation/navigational_arrows_press_right" );
    QPixmapCache::remove( "marble/navigation/navigational_arrows_press_top" );
}

void ArrowDiscWidget::setMarbleWidget( MarbleWidget *marbleWidget )
{
    m_parentMarbleWidget = marbleWidget;
}

QPixmap ArrowDiscWidget::pixmap( const QString &Id )
{
    QPixmap result;
    if ( !QPixmapCache::find( Id, result ) ) {
        result = QPixmap::fromImage( QImage( QString( ":/%1.png" ).arg( Id ) ) );
        QPixmapCache::insert( Id, result );
    }
    return result;
}

void ArrowDiscWidget::mousePressEvent( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() == Qt::LeftButton ) {
        QPointF mousePos = mouseEvent->pos();

        // mouse coordinates relative to widget topleft
        int mx = mousePos.x();
        int my = mousePos.y();

        // center coordinates relative to widget topleft
        int cx = width()/2;
        int cy = height()/2;

        int px = mx - cx;
        int py = my - cy;

        qreal distance = sqrt( px*px + py*py );

        if ( distance >= min_radius && distance <= max_radius ) {
            if ( pathNorth.contains( mousePos ) ) {
                m_imagePath = "marble/navigation/navigational_arrows_press_top";
                m_parentMarbleWidget->moveUp();
            } else if ( pathSouth.contains( mousePos ) ) {
                m_imagePath = "marble/navigation/navigational_arrows_press_bottom";
                m_parentMarbleWidget->moveDown();
            } else if ( pathWest.contains( mousePos ) ) {
                m_imagePath = "marble/navigation/navigational_arrows_press_left";
                m_parentMarbleWidget->moveLeft();
            } else if ( pathEast.contains( mousePos ) ) {
                m_imagePath = "marble/navigation/navigational_arrows_press_right";
                m_parentMarbleWidget->moveRight();
            }
        } else {
            m_imagePath = "marble/navigation/navigational_arrows";
        }
        repaint();
    }
}

void ArrowDiscWidget::mouseReleaseEvent( QMouseEvent *mouseEvent )
{
    mouseMoveEvent( mouseEvent );
}

void ArrowDiscWidget::mouseMoveEvent( QMouseEvent *mouseEvent )
{
    QPointF mousePos = mouseEvent->pos();

    // mouse coordinates relative to widget topleft
    int mx = mousePos.x();
    int my = mousePos.y();

    // center coordinates relative to widget topleft
    int cx = width()/2;
    int cy = height()/2;

    int px = mx - cx;
    int py = my - cy;

    qreal distance = sqrt( px*px + py*py );

    if ( distance >= min_radius && distance <= max_radius ) {
        if ( pathNorth.contains( mousePos ) ) {
            m_imagePath = "marble/navigation/navigational_arrows_hover_top";
        } else if ( pathSouth.contains( mousePos ) ) {
            m_imagePath = "marble/navigation/navigational_arrows_hover_bottom";
        } else if ( pathWest.contains( mousePos ) ) {
            m_imagePath = "marble/navigation/navigational_arrows_hover_left";
        } else if ( pathEast.contains( mousePos ) ) {
            m_imagePath = "marble/navigation/navigational_arrows_hover_right";
        }
    } else {
        m_imagePath = "marble/navigation/navigational_arrows";
    }
    repaint();
}

void ArrowDiscWidget::resizeEvent( QResizeEvent *event )
{
    QWidget::resizeEvent( event );

    pathNorth.moveTo( width()/2, height()/2 );
    pathNorth.arcTo( 0.0, 0.0, 70.0, 70.0, 45.0, 90.0 );
    pathNorth.closeSubpath();
    pathSouth.moveTo( width()/2, height()/2 );
    pathSouth.arcTo( 0.0, 0.0, 70.0, 70.0, 225.0, 90.0 );
    pathSouth.closeSubpath();
    pathWest.moveTo( width()/2, height()/2 );
    pathWest.arcTo( 0.0, 0.0, 70.0, 70.0, 135.0, 90.0 );
    pathWest.closeSubpath();
    pathEast.moveTo( width()/2, height()/2 );
    pathEast.arcTo( 0.0, 0.0, 70.0, 70.0, 315.0, 90.0 );
    pathEast.closeSubpath();
}

void ArrowDiscWidget::repaint()
{
    emit repaintNeeded();
}

void ArrowDiscWidget::paintEvent( QPaintEvent * )
{
    Q_ASSERT( !pixmap( m_imagePath ).isNull() );
    QPainter painter( this );
    painter.drawPixmap( 0, 0, pixmap( m_imagePath ) );
    painter.end();
}

}

#include "ArrowDiscWidget.moc"
