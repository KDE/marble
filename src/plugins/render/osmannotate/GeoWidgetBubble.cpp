#include "GeoWidgetBubble.h"

#include "ViewportParams.h"
#include "GeoPainter.h"
#include "AbstractProjection.h"

#include <QtGui/QWidget>

namespace Marble{

GeoWidgetBubble::GeoWidgetBubble()
{
    setGeoOffset(true);
    widget =0;
    initalised = false;
}

GeoWidgetBubble::GeoWidgetBubble( TmpGraphicsItem* parent )
{
    setParent( parent );
    setGeoOffset(true);
    setCoordinate( GeoDataCoordinates( 50, 50 ) );
    widget =0;
    initalised = false;
}

void GeoWidgetBubble::paint( GeoPainter* painter, ViewportParams* view,
                             const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if( !initalised && ( widget !=0)  ) {
        initWidget( (QWidget* ) painter->device() );
    }



    if ( initalised && ( widget != 0 ) ) {

    qreal x, y;
    bool behind, valid;

    valid = view->currentProjection()->screenCoordinates( getParent()->coordinate(), view,
                                                          x, y, behind );


    if( !behind && valid ) {
        widget->setVisible( true );
        painter->save();
        QSize widgetSize = widget->size();

        //how wide and high the border is
        //sum of both sides of the border
        QSize borderSize( 40, 40 );

        QPoint borderOffset( -20, -20 );

        //offset from the placemark
        QPoint offset( coordinate().longitude(), coordinate().latitude() );
        //position of the bubble
        QPoint position = QPoint(x, y) + offset;
        widget->move( position ) ;

        painter->drawText(position, "HELLO!!!!");

        //draw the border
        painter->setBrush( QBrush( Qt::blue, Qt::SolidPattern ));
        painter->drawRoundedRect( QRect( position + borderOffset, widgetSize + borderSize ),
                                  50, 50 , Qt::RelativeSize );

        //draw the "Widget"
//        painter->setBrush( QBrush( Qt::yellow, Qt::SolidPattern ) );
//        painter->drawRect( QRect( position, widgetSize ) );
        painter->restore();
        } else {
            widget->setVisible( false );
        }

    }

}

void GeoWidgetBubble::setWidget( QWidget* w )
{
    widget = w;
    widget->setVisible(false);
}

QWidget* GeoWidgetBubble::getWidget()
{
    return widget;
}

void GeoWidgetBubble::initWidget( QWidget* parent )
{
    widget->setParent( parent );
    widget->setVisible( true );
    initalised = true;
}

bool GeoWidgetBubble::isInitalised()
{
    return initalised;
}

}
