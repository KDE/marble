//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "NavigationSlider.h"

#include <QPainter>
#include <QMouseEvent>
#include <QPixmapCache>

namespace Marble
{

namespace {
    const int handleImageHeight = 32;
}

NavigationSlider::NavigationSlider(QWidget *parent) :
    QAbstractSlider( parent ),
    m_handleImagePath( "marble/navigation/navigational_slider_handle" )
{
    setMouseTracking( true );
}

NavigationSlider::~NavigationSlider()
{
    QPixmapCache::remove( "marble/navigation/navigational_slider_groove" );
    QPixmapCache::remove( "marble/navigation/navigational_slider_handle" );
    QPixmapCache::remove( "marble/navigation/navigational_slider_handle_hover" );
    QPixmapCache::remove( "marble/navigation/navigational_slider_handle_press" );
}

QPixmap NavigationSlider::pixmap( const QString &id )
{
    QPixmap result;
    if ( !QPixmapCache::find( id, result ) ) {
        result = QPixmap( QString( ":/%1.png" ).arg( id ) );
        QPixmapCache::insert( id, result );
    }
    return result;
}

void NavigationSlider::enterEvent( QEvent * )
{
    setSliderDown( false );
    if ( m_handleImagePath != "marble/navigation/navigational_slider_handle_hover" ) {
        m_handleImagePath = "marble/navigation/navigational_slider_handle_hover";
        repaint();
    }
}

void NavigationSlider::mouseMoveEvent( QMouseEvent *mouseEvent )
{
    if ( !isSliderDown() && mouseEvent->buttons() & Qt::LeftButton ) {
        setSliderDown( true );
    }
    if ( isSliderDown() ) {
        qreal const fraction = ( mouseEvent->pos().y() - handleImageHeight/2 ) / qreal ( height() - handleImageHeight );
        int v = ( int ) minimum() + ( ( maximum() - minimum() ) ) * ( 1 - fraction );
        setValue( v );
        repaint();
    }
}

void NavigationSlider::mousePressEvent( QMouseEvent * )
{
    setSliderDown( true );
    if ( m_handleImagePath != "marble/navigation/navigational_slider_handle_press" ) {
        m_handleImagePath = "marble/navigation/navigational_slider_handle_press";
        repaint();
    }
}

void NavigationSlider::mouseReleaseEvent( QMouseEvent * )
{
    setSliderDown( false );
    if ( m_handleImagePath != "marble/navigation/navigational_slider_handle_hover" ) {
        m_handleImagePath = "marble/navigation/navigational_slider_handle_hover";
        repaint();
    }
}

void NavigationSlider::leaveEvent( QEvent * )
{
    setSliderDown( false );
    if ( m_handleImagePath != "marble/navigation/navigational_slider_handle" ) {
        m_handleImagePath = "marble/navigation/navigational_slider_handle";
        repaint();
    }
}

void NavigationSlider::repaint()
{
    emit repaintNeeded();
}

void NavigationSlider::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    for ( int y = 0; y <= 160; y+=10 ) {
        painter.drawPixmap( 0, y, pixmap( "marble/navigation/navigational_slider_groove" ) );

    }
    qreal const fraction = ( value() - minimum() ) / qreal( maximum() - minimum() );
    int const y = ( height() - handleImageHeight ) * ( 1 - fraction );
    painter.drawPixmap( 0, y, pixmap( m_handleImagePath ) );
    painter.end();
}

}

#include "NavigationSlider.moc"
