//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//


#include "MarbleAbstractFloatItem.h"


#include <QtCore/QDebug>
#include <QtCore/QRectF>
#include "GeoPainter.h"

class MarbleAbstractFloatItemPrivate
{
  public:
    MarbleAbstractFloatItemPrivate( const QPointF &point, const QSizeF &size )
        : m_position( point ),
          m_size( size ),
          m_visible( true ), 
          m_newItemProperties( true )
    {
        calculateLayout();
    }

    ~MarbleAbstractFloatItemPrivate()
    {
    }

    void calculateLayout()
    {
        double marginTop = ( s_marginTop == 0 ) ? s_margin : s_marginTop;;
        double marginBottom = ( s_marginBottom == 0 ) ? s_margin : s_marginBottom;;
        double marginLeft = ( s_marginLeft == 0 ) ? s_margin : s_marginLeft;
        double marginRight = ( s_marginRight == 0 ) ? s_margin : s_marginRight;;

        m_renderedRect = QRectF( m_position.x() + marginLeft, m_position.y() + marginTop, m_size.width() - ( marginLeft + marginRight ), m_size.height() - ( marginTop + marginBottom ) ); 
    
        m_contentRect = QRectF( 
            m_position.x() + marginLeft + s_padding, 
            m_position.y() + marginTop + s_padding, 
            m_size.width() - ( marginLeft + marginRight + 2 * s_padding ), 
            m_size.height() - ( marginTop + marginBottom + 2 * s_padding ) 
        ); 
    }

    QPointF             m_position;
    QSizeF              m_size;
    bool                m_visible;

    QRectF              m_renderedRect;
    QRectF              m_contentRect;

    static QPen         s_pen;
    static QBrush       s_background;
    static double       s_border;
    static QBrush       s_borderBrush;
    static Qt::PenStyle s_borderStyle;
    static double       s_margin;
    static double       s_marginTop;
    static double       s_marginBottom;
    static double       s_marginLeft;
    static double       s_marginRight;
    static double       s_padding;
    static bool         s_pixmapCacheEnabled;

    QPixmap             m_cachePixmap;
    bool                m_newItemProperties;
};

QPen         MarbleAbstractFloatItemPrivate::s_pen = QPen( Qt::black );
QBrush       MarbleAbstractFloatItemPrivate::s_background = QBrush( QColor( 192, 192, 192, 192 ) );
double       MarbleAbstractFloatItemPrivate::s_border = 1.0;
QBrush       MarbleAbstractFloatItemPrivate::s_borderBrush = QBrush( Qt::black );
Qt::PenStyle MarbleAbstractFloatItemPrivate::s_borderStyle = Qt::SolidLine;
double       MarbleAbstractFloatItemPrivate::s_margin = 0.0;
double       MarbleAbstractFloatItemPrivate::s_marginTop = 0.0;
double       MarbleAbstractFloatItemPrivate::s_marginBottom = 0.0;
double       MarbleAbstractFloatItemPrivate::s_marginLeft = 0.0;
double       MarbleAbstractFloatItemPrivate::s_marginRight = 0.0;
double       MarbleAbstractFloatItemPrivate::s_padding = 4.0;
bool         MarbleAbstractFloatItemPrivate::s_pixmapCacheEnabled = true;

MarbleAbstractFloatItem::MarbleAbstractFloatItem( const QPointF &point, 
                                                  const QSizeF &size )
    : d( new MarbleAbstractFloatItemPrivate( point, size ) )
{
}

MarbleAbstractFloatItem::~MarbleAbstractFloatItem()
{
    delete d;
}

void MarbleAbstractFloatItem::setPosition( const QPointF& position )
{
    d->m_position = position;

    d->calculateLayout();
}

QPointF MarbleAbstractFloatItem::position() const
{
    return d->m_position;
}

void MarbleAbstractFloatItem::setSize( const QSizeF& size )
{
    d->m_size = size;

    d->calculateLayout();
    d->m_newItemProperties = true;
}

QSizeF MarbleAbstractFloatItem::size() const
{
    return d->m_size;
}

void MarbleAbstractFloatItem::setVisible( bool visible )
{
    d->m_visible = visible;
}

bool MarbleAbstractFloatItem::visible() const
{
    return d->m_visible;
}

QPen MarbleAbstractFloatItem::pen() const
{
    return d->s_pen;
}

void MarbleAbstractFloatItem::setPen( const QPen &pen )
{
    d->s_pen = pen;
    d->m_newItemProperties = true;
}

QBrush MarbleAbstractFloatItem::background() const
{
    return d->s_background;
}

void MarbleAbstractFloatItem::setBackground( const QBrush &background )
{
    d->s_background = background;
    d->m_newItemProperties = true;
}

QRectF MarbleAbstractFloatItem::contentRect() const
{
    return d->m_contentRect;
}

QRectF MarbleAbstractFloatItem::renderedRect() const
{
    return d->m_renderedRect;
}

QPainterPath MarbleAbstractFloatItem::backgroundShape() const
{
    QPainterPath path;
    path.addRect( d->m_renderedRect );
    return path;
}

double MarbleAbstractFloatItem::border() const
{
    return d->s_border;
}

void MarbleAbstractFloatItem::setBorder( double border )
{
    d->s_border = border;
    d->m_newItemProperties = true;
}

QBrush MarbleAbstractFloatItem::borderBrush() const
{
    return d->s_borderBrush;
}

void MarbleAbstractFloatItem::setBorderBrush( const QBrush &borderBrush )
{
    d->s_borderBrush = borderBrush;
    d->m_newItemProperties = true;
}

Qt::PenStyle MarbleAbstractFloatItem::borderStyle () const
{
    return d->s_borderStyle;
} 

void MarbleAbstractFloatItem::setBorderStyle( Qt::PenStyle borderStyle )
{
    d->s_borderStyle = borderStyle;
    d->m_newItemProperties = true;
}

double MarbleAbstractFloatItem::margin() const
{
    return d->s_margin;
} 

void MarbleAbstractFloatItem::setMargin( double margin )
{
    d->s_margin = margin;

    d->calculateLayout();
    d->m_newItemProperties = true;
}

double MarbleAbstractFloatItem::marginTop() const
{
    return d->s_marginTop;
} 

void MarbleAbstractFloatItem::setMarginTop( double marginTop )
{
    d->s_marginTop = marginTop;

    d->calculateLayout();
    d->m_newItemProperties = true;
}

double MarbleAbstractFloatItem::marginBottom() const
{
    return d->s_marginBottom;
} 

void MarbleAbstractFloatItem::setMarginBottom( double marginBottom )
{
    d->s_marginBottom = marginBottom;

    d->calculateLayout();
    d->m_newItemProperties = true;
}

double MarbleAbstractFloatItem::marginLeft() const
{
    return d->s_marginLeft;
} 

void MarbleAbstractFloatItem::setMarginLeft( double marginLeft )
{
    d->s_marginLeft = marginLeft;

    d->calculateLayout();
    d->m_newItemProperties = true;
}

double MarbleAbstractFloatItem::marginRight() const
{
    return d->s_marginRight;
} 

void MarbleAbstractFloatItem::setMarginRight( double marginRight )
{
    d->s_marginRight = marginRight;

    d->calculateLayout();
    d->m_newItemProperties = true;
}

double MarbleAbstractFloatItem::padding () const
{
    return d->s_padding;
}

void MarbleAbstractFloatItem::setPadding( double padding )
{
    d->s_padding = padding;

    d->calculateLayout();
    d->m_newItemProperties = true;
}

bool MarbleAbstractFloatItem::needsUpdate( ViewportParams *viewport )
{
    return false;
}

bool MarbleAbstractFloatItem::pixmapCacheEnabled() const
{
    return d->s_pixmapCacheEnabled;
}

void MarbleAbstractFloatItem::setPixmapCacheEnabled( bool pixmapCacheEnabled )
{
    d->s_pixmapCacheEnabled = pixmapCacheEnabled;
    d->m_newItemProperties = true;
}

QString MarbleAbstractFloatItem::renderPolicy() const 
{
    return "ALWAYS";
}

QStringList MarbleAbstractFloatItem::renderPosition() const {
    return QStringList( "FLOAT_ITEM" );
}

bool MarbleAbstractFloatItem::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer)
{
//    qDebug() << "renderPos: " << renderPos;
    bool success = true;

    if ( renderPos == "FLOAT_ITEM" ) {
        // Prevent unneeded redraws
        if ( !needsUpdate( viewport ) && d->s_pixmapCacheEnabled && !d->m_newItemProperties ) {
            painter->drawPixmap( d->m_position, d->m_cachePixmap );
            return true;
        }
    
        // Reinitialize cachePixmap if the float item changes its size 
        // or other important common properties 
        if ( d->s_pixmapCacheEnabled && d->m_newItemProperties ) {
            // Add extra space for the border
            QSize cachePixmapSize = d->m_size.toSize() + QSize( 1, 1 );
            d->m_cachePixmap = QPixmap( cachePixmapSize );
        }
        // unset the dirty flag once all checks are passed
        d->m_newItemProperties = false;
    
        // Clear the pixmap and redirect the painter
        QPaintDevice* mapDevice = painter->device();
    
        if ( d->s_pixmapCacheEnabled ) {
            d->m_cachePixmap.fill( Qt::transparent );
            painter->end();
            GeoPainter::setRedirected( mapDevice, &( d->m_cachePixmap ) );
            painter->begin( &( d->m_cachePixmap ) );
            painter->translate( -d->m_position.x(), -d->m_position.y() );
        }
    
        painter->setPen( QPen( d->s_borderBrush, d->s_border, d->s_borderStyle ) );
        painter->setBrush( d->s_background );
        painter->drawPath( backgroundShape() );
    
        success = renderFloatItem( painter, viewport, layer );
    
        if ( d->s_pixmapCacheEnabled ) {
            painter->end();
            GeoPainter::restoreRedirected( mapDevice );
            painter->begin( mapDevice );
            painter->drawPixmap( d->m_position, d->m_cachePixmap );
        }
    }
    else {
        success = renderOnMap( painter, viewport, renderPos, layer );
    }

    return success;
}

bool MarbleAbstractFloatItem::renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer *layer )
{
    // In the derived method here is the right place to draw the contents of the float item

    return true;
}

bool MarbleAbstractFloatItem::renderOnMap( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer *layer )
{
    // In the derived method here is the place where you can draw some additional stuff onto 
    // the map itself

    return true;
}
