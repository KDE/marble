//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoWidgetBubble.h"

#include "GeoPainter.h"

#include <QtGui/QWidget>

namespace Marble
{

GeoWidgetBubble::GeoWidgetBubble()
{
    m_widget=0;
    marbleWidgetInitalised = false;
    m_offset = QPoint( 10, 10 );
    m_hidden = true;
}

void GeoWidgetBubble::paint( GeoPainter* painter, ViewportParams* viewport,
                             const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );
    Q_UNUSED( viewport );

    if( !marbleWidgetInitalised && ( m_widget!=0)  ) {
        initaliseMarbleWidget( (QWidget* ) painter->device() );
    }

    if( !m_hidden ) {

    if ( marbleWidgetInitalised && ( m_widget!= 0 ) ) {

        m_widget->setVisible( true );
        QSize widgetSize = m_widget->size();
        //how wide and high the border is
        //sum of both sides of the border
        QSize borderSize( 40, 40 );
        QPoint borderOffset( -20, -20 );

        //position of the bubble
        QPoint position =  m_screenPosition + m_offset;
        m_widget->move( position ) ;

        painter->save();

        //draw the border
        painter->setPen( QPen( QColor( 125, 125, 125) ) );
        painter->setBrush( QBrush( QColor( 255, 255, 255) , Qt::SolidPattern ));
        painter->drawRoundedRect( QRect( position + borderOffset, widgetSize + borderSize ),
                                  30, 30  );

        painter->restore();
        }
    }
    else {
        m_widget->hide();
    }

}

void GeoWidgetBubble::setGeoWidget( QWidget* w )
{
    QPalette p(Qt::lightGray, Qt::lightGray);

    m_widget= w;
    m_widget->setVisible(false);
    m_widget->setPalette( p );
}

QWidget* GeoWidgetBubble::getGeoWidget()
{
    return m_widget;
}

void GeoWidgetBubble::initaliseMarbleWidget( QWidget* parent )
{
    m_widget->setParent( parent );
    m_widget->setVisible( true );
    marbleWidgetInitalised = true;
}

bool GeoWidgetBubble::marbleWidgetIsInitalised() const
{
    return marbleWidgetInitalised;
}

void GeoWidgetBubble::moveTo( QPoint pos )
{
     m_screenPosition = pos;
}

void GeoWidgetBubble::setHidden( bool hide )
{
    if( m_hidden == hide ) {
        return;
    }

    //if its not hidden and we want to hide
    if( hide && !m_hidden ) {
        m_widget->setVisible( m_hidden );
        m_hidden = true ;
    }
    else if (  !hide && m_hidden ) {
        m_hidden = false;
    }

    if ( marbleWidgetInitalised ) {
        m_widget->parentWidget()->update();
    }
}

bool GeoWidgetBubble::isHidden() const
{
    return m_hidden;
}

}
