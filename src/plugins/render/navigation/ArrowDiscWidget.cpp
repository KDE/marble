//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "ArrowDiscWidget.h"

#include "MarbleWidget.h"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmapCache>
#include <QtGui/QPainterPath>

namespace Marble
{

ArrowDiscWidget::ArrowDiscWidget( QWidget *parent ) :
    QWidget( parent ),
    m_marbleWidget( 0 ),
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
    m_marbleWidget = marbleWidget;
}

QPixmap ArrowDiscWidget::pixmap( const QString &id )
{
    QPixmap result;
    if ( !QPixmapCache::find( id, result ) ) {
        result = QPixmap( QString( ":/%1.png" ).arg( id ) );
        QPixmapCache::insert( id, result );
    }
    return result;
}

void ArrowDiscWidget::mousePressEvent( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() == Qt::LeftButton ) {
        switch ( arrowUnderMouse( mouseEvent->pos() ) ) {
        case Qt::NoArrow:
            m_imagePath = "marble/navigation/navigational_arrows";
            break;
        case Qt::UpArrow:
            m_imagePath = "marble/navigation/navigational_arrows_press_top";
            m_marbleWidget->moveUp();
            break;
        case Qt::DownArrow:
            m_imagePath = "marble/navigation/navigational_arrows_press_bottom";
            m_marbleWidget->moveDown();
            break;
        case Qt::LeftArrow:
            m_imagePath = "marble/navigation/navigational_arrows_press_left";
            m_marbleWidget->moveLeft();
            break;
        case Qt::RightArrow:
            m_imagePath = "marble/navigation/navigational_arrows_press_right";
            m_marbleWidget->moveRight();
            break;
        }
    }

    repaint();
}

void ArrowDiscWidget::mouseReleaseEvent( QMouseEvent *mouseEvent )
{
    mouseMoveEvent( mouseEvent );
}

void ArrowDiscWidget::leaveEvent( QEvent* )
{
    if ( m_imagePath != "marble/navigation/navigational_arrows" ) {
        m_imagePath = "marble/navigation/navigational_arrows";
        repaint();
    }
}

void ArrowDiscWidget::mouseMoveEvent( QMouseEvent *mouseEvent )
{
    QString const oldPath = m_imagePath;
    switch ( arrowUnderMouse( mouseEvent->pos() ) ) {
    case Qt::NoArrow:
        m_imagePath = "marble/navigation/navigational_arrows";
        break;
    case Qt::UpArrow:
        m_imagePath = "marble/navigation/navigational_arrows_hover_top";
        break;
    case Qt::DownArrow:
        m_imagePath = "marble/navigation/navigational_arrows_hover_bottom";
        break;
    case Qt::LeftArrow:
        m_imagePath = "marble/navigation/navigational_arrows_hover_left";
        break;
    case Qt::RightArrow:
        m_imagePath = "marble/navigation/navigational_arrows_hover_right";
        break;
    }

    if ( m_imagePath != oldPath ) {
        repaint();
    }
}

void ArrowDiscWidget::repaint()
{
    emit repaintNeeded();
}

Qt::ArrowType ArrowDiscWidget::arrowUnderMouse(const QPoint &position) const
{
    const int min_radius_pow2 = 5*5;
    const int max_radius_pow2 = 28*28;

    // mouse coordinates relative to widget topleft
    int mx = position.x();
    int my = position.y();

    // center coordinates relative to widget topleft
    int cx = width()/2;
    int cy = height()/2;

    int px = mx - cx;
    int py = my - cy;

    int const distance_pow2 = px*px + py*py;

    if ( distance_pow2 >= min_radius_pow2 && distance_pow2 <= max_radius_pow2 ) {
        int const angle = int( atan2( py, px ) * RAD2DEG );
        Q_ASSERT( -180 <= angle && angle <= 180 );

        if ( angle >= 135 || angle < -135 ) {
            return Qt::LeftArrow;
        } else if ( angle < -45 ) {
            return Qt::UpArrow;
        } else if ( angle < 45 ) {
            return Qt::RightArrow;
        } else {
            return Qt::DownArrow;
        }
    }

    return Qt::NoArrow;
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
