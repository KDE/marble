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

#include <qmath.h>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmapCache>

namespace Marble
{

ArrowDiscWidget::ArrowDiscWidget( QWidget *parent ) :
    QWidget( parent ),
    m_arrowPressed( Qt::NoArrow ),
    m_repetitions( 0 ),
    m_marbleWidget( 0 ),
    m_imagePath( "marble/navigation/navigational_arrows" )
{
    setMouseTracking( true );

    m_initialPressTimer.setSingleShot( true );
    connect( &m_initialPressTimer, SIGNAL(timeout()), SLOT(startPressRepeat()) );
    connect( &m_repeatPressTimer, SIGNAL(timeout()), SLOT(repeatPress()) );
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
        result = QPixmap(QLatin1String(":/") + id + QLatin1String(".png"));
        QPixmapCache::insert( id, result );
    }
    return result;
}

void ArrowDiscWidget::mousePressEvent( QMouseEvent *mouseEvent )
{
    if ( mouseEvent->button() == Qt::LeftButton ) {

        if ( !m_initialPressTimer.isActive() && !m_repeatPressTimer.isActive() ) {
            m_repetitions = 0;
            m_initialPressTimer.start( 300 );
        }

        m_arrowPressed = arrowUnderMouse( mouseEvent->pos() );
        switch ( m_arrowPressed ) {
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
    m_initialPressTimer.stop();
    m_repeatPressTimer.stop();
    mouseMoveEvent( mouseEvent );
}

void ArrowDiscWidget::leaveEvent( QEvent* )
{
    if (m_imagePath != QLatin1String("marble/navigation/navigational_arrows")) {
        m_imagePath = "marble/navigation/navigational_arrows";
        repaint();
    }

    m_initialPressTimer.stop();
    m_repeatPressTimer.stop();
}

void ArrowDiscWidget::startPressRepeat()
{
    repeatPress();

    if ( m_arrowPressed != Qt::NoArrow ) {
        m_repeatPressTimer.start( 100 );
    }
}

void ArrowDiscWidget::repeatPress()
{
    if ( m_repetitions <= 200 ) {
        ++m_repetitions;
        switch ( m_arrowPressed ) {
        case Qt::NoArrow:
            break;
        case Qt::UpArrow:
            m_marbleWidget->moveUp();
            break;
        case Qt::DownArrow:
            m_marbleWidget->moveDown();
            break;
        case Qt::LeftArrow:
            m_marbleWidget->moveLeft();
            break;
        case Qt::RightArrow:
            m_marbleWidget->moveRight();
            break;
        }
    } else {
        m_repeatPressTimer.stop();
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
        m_arrowPressed = Qt::UpArrow;
        break;
    case Qt::DownArrow:
        m_imagePath = "marble/navigation/navigational_arrows_hover_bottom";
        m_arrowPressed = Qt::DownArrow;
        break;
    case Qt::LeftArrow:
        m_imagePath = "marble/navigation/navigational_arrows_hover_left";
        m_arrowPressed = Qt::LeftArrow;
        break;
    case Qt::RightArrow:
        m_imagePath = "marble/navigation/navigational_arrows_hover_right";
        m_arrowPressed = Qt::RightArrow;
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
        int const angle = int( qAtan2( py, px ) * RAD2DEG );
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

#include "moc_ArrowDiscWidget.cpp"
