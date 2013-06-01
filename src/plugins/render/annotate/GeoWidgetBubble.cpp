//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
//

#include "GeoWidgetBubble.h"

#include "TextEditor.h"

#include "MarbleColors.h"

#include <QtGui/QPainter>
#include <QtGui/QWidget>

namespace Marble
{

GeoWidgetBubble::GeoWidgetBubble( GeoDataPlacemark *placemark )
    : m_widget( new TextEditor( placemark ) ),
      m_hidden( true ),
      m_widgetInitialized( false ),
      m_offset( QPoint( 10, 10 ) )
{
    m_widget->setVisible(false);
    m_widget->setPalette( QPalette(Qt::lightGray, Qt::lightGray) );
}

GeoWidgetBubble::~GeoWidgetBubble()
{
    delete m_widget;
}

void GeoWidgetBubble::paint( QPainter* painter )
{
    if( !m_widgetInitialized && ( m_widget!=0)  ) {
        QWidget *widget = dynamic_cast<QWidget*>( painter->device() );
        if ( widget ) {
            setParentWidget( widget );
        }
    }

    if( !m_hidden ) {

    if ( m_widgetInitialized ) {

        m_widget->setVisible( true );
        QSize widgetSize = m_widget->size();
        //how wide and high the border is
        //sum of both sides of the border
        QSize borderSize( 40, 40 );
        QPoint borderOffset( -10, -10 );

        //position of the bubble
        QPoint position =  m_screenPosition + m_offset;
        m_widget->move( position ) ;

        painter->save();

        //draw the border
        painter->setPen( QPen( Oxygen::aluminumGray4 ) );
        painter->setBrush( QBrush( QColor( 255, 255, 255) , Qt::SolidPattern ));
        painter->drawRoundedRect( QRect( position + borderOffset, widgetSize + borderSize ),
                                  10, 10  );

        painter->restore();
        }
    }
    else {
        m_widget->hide();
    }

}

void GeoWidgetBubble::setParentWidget( QWidget* parent )
{
    m_widget->setParent( parent );
    m_widget->setVisible( true );
    m_widgetInitialized = true;
}

void GeoWidgetBubble::moveTo( const QPoint &pos )
{
     m_screenPosition = pos;
}

void GeoWidgetBubble::setHidden( bool hide )
{
    if( m_hidden == hide ) {
        return;
    }

    m_hidden = hide;
    m_widget->setVisible( m_hidden );

    if ( m_widgetInitialized ) {
        m_widget->parentWidget()->update();
    }
}

bool GeoWidgetBubble::isHidden() const
{
    return m_hidden;
}

}
