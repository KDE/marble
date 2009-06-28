#include "GeoWidgetBubble.h"

#include "GeoPainter.h"

#include <QtGui/QWidget>

namespace Marble{

GeoWidgetBubble::GeoWidgetBubble()
{
    m_widget=0;
    marbleWidgetInitalised = false;
    m_offset = QPoint( 10, 10 );
}

void GeoWidgetBubble::paint( GeoPainter* painter, ViewportParams* view,
                             const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if( !marbleWidgetInitalised && ( m_widget!=0)  ) {
        initaliseMarbleWidget( (QWidget* ) painter->device() );
    }

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
        painter->setBrush( QBrush( Qt::blue, Qt::SolidPattern ));
        painter->drawRoundedRect( QRect( position + borderOffset, widgetSize + borderSize ),
                                  50, 50 , Qt::RelativeSize );

        painter->restore();
        }
}

void GeoWidgetBubble::setGeoWidget( QWidget* w )
{
    m_widget= w;
    m_widget->setVisible(false);
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

bool GeoWidgetBubble::marbleWidgetIsInitalised()
{
    return marbleWidgetInitalised;
}

void GeoWidgetBubble::moveTo( QPoint pos )
{
     m_screenPosition = pos;
}

void GeoWidgetBubble::hide()
{
    m_widget->hide();
}

}
