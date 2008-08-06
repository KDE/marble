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
#include "MarbleWidget.h"
#include "MarbleMap.h"

#include <QtGui/QAction>
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
        double marginTop = ( s_marginTop == 0.0 ) ? s_margin : s_marginTop;;
        double marginBottom = ( s_marginBottom == 0.0 ) ? s_margin : s_marginBottom;;
        double marginLeft = ( s_marginLeft == 0.0 ) ? s_margin : s_marginLeft;
        double marginRight = ( s_marginRight == 0.0 ) ? s_margin : s_marginRight;;

        m_renderedRect = QRectF( m_position.x() + marginLeft, m_position.y() + marginTop, m_size.width() - ( marginLeft + marginRight ), m_size.height() - ( marginTop + marginBottom ) ); 
    
        m_contentRect = QRectF( 
            m_position.x() + marginLeft + s_padding, 
            m_position.y() + marginTop + s_padding, 
            m_size.width() - ( marginLeft + marginRight + 2.0 * s_padding ), 
            m_size.height() - ( marginTop + marginBottom + 2.0 * s_padding ) 
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
    static QFont        s_font;
    static double       s_margin;
    static double       s_marginTop;
    static double       s_marginBottom;
    static double       s_marginLeft;
    static double       s_marginRight;
    static double       s_padding;
    static bool         s_pixmapCacheEnabled;

    QPixmap             m_cachePixmap;
    bool                m_newItemProperties;

    QPoint              m_floatItemMoveStartPos;
};

QPen         MarbleAbstractFloatItemPrivate::s_pen = QPen( Qt::black );
QBrush       MarbleAbstractFloatItemPrivate::s_background = QBrush( QColor( 192, 192, 192, 192 ) );
double       MarbleAbstractFloatItemPrivate::s_border = 1.0;
QBrush       MarbleAbstractFloatItemPrivate::s_borderBrush = QBrush( Qt::black );
Qt::PenStyle MarbleAbstractFloatItemPrivate::s_borderStyle = Qt::SolidLine;
#ifdef Q_OS_MACX
    QFont MarbleAbstractFloatItemPrivate::s_font = QFont( "Sans Serif", 10 );
#else
    QFont MarbleAbstractFloatItemPrivate::s_font = QFont( "Sans Serif", 8 );
#endif
double       MarbleAbstractFloatItemPrivate::s_margin = 0.0;
double       MarbleAbstractFloatItemPrivate::s_marginTop = 0.0;
double       MarbleAbstractFloatItemPrivate::s_marginBottom = 0.0;
double       MarbleAbstractFloatItemPrivate::s_marginLeft = 0.0;
double       MarbleAbstractFloatItemPrivate::s_marginRight = 0.0;
double       MarbleAbstractFloatItemPrivate::s_padding = 4.0;
bool         MarbleAbstractFloatItemPrivate::s_pixmapCacheEnabled = true;

MarbleAbstractFloatItem::MarbleAbstractFloatItem( const QPointF &point, 
                                                  const QSizeF &size )
    : MarbleAbstractLayer(),
      d( new MarbleAbstractFloatItemPrivate( point, size ) )
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

QPointF MarbleAbstractFloatItem::positivePosition( const QRectF& viewPort ) const
{
    double x, y;
    x = ( d->m_position.x() < 0 ) ? viewPort.width() - d->m_size.width() + d->m_position.x() : d->m_position.x();
    y = ( d->m_position.y() < 0 ) ? viewPort.height() - d->m_size.height() + d->m_position.y() : d->m_position.y();

    return QPointF( x, y );
}

void MarbleAbstractFloatItem::setSize( const QSizeF& size )
{
    if ( size == d->m_size )
        return;

    d->m_size = size;

    d->calculateLayout();
    d->m_newItemProperties = true;
}

QSizeF MarbleAbstractFloatItem::size() const
{
    return d->m_size;
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
    path.addRect( QRectF( 0.0, 0.0, d->m_renderedRect.size().width() - 1, d->m_renderedRect.size().height() - 1 ) );
    return path;
}

void MarbleAbstractFloatItem::renderBackground( QPainter* painter )
{
    painter->save();

    painter->setPen( QPen( d->s_borderBrush, d->s_border, d->s_borderStyle ) );
    painter->setBrush( d->s_background );
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->translate( QPointF( marginLeft(), marginTop() ) );
    painter->drawPath( backgroundShape() );

    painter->restore();
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

QFont MarbleAbstractFloatItem::font() const
{
    return d->s_font;
}

void MarbleAbstractFloatItem::setFont( const QFont &font )
{
    d->s_font = font;
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
    Q_UNUSED( viewport );

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
            painter->drawPixmap( positivePosition( painter->viewport() ), d->m_cachePixmap );
            return true;
        }
    
        // Reinitialize cachePixmap if the float item changes its size 
        // or other important common properties 
        if ( ( d->s_pixmapCacheEnabled && d->m_newItemProperties ) || d->m_cachePixmap.isNull() ) {
            // Add extra space for the border
            QSize cachePixmapSize = d->m_size.toSize();


            if ( d->m_size.isValid() && !d->m_size.isNull() ) {
                d->m_cachePixmap = QPixmap( cachePixmapSize ).copy();
            }
            else {
                qDebug() << "Warning: Invalid pixmap size suggested: " << d->m_size;
            }
        }
        // unset the dirty flag once all checks are passed
        d->m_newItemProperties = false;

        if ( d->s_pixmapCacheEnabled ) {
            d->m_cachePixmap.fill( Qt::transparent );
            GeoPainter pixmapPainter( &( d->m_cachePixmap ), viewport, Normal );

            pixmapPainter.translate( 0.5, 0.5 );
            renderBackground( &pixmapPainter );

            pixmapPainter.translate( d->s_padding, d->s_padding );

            pixmapPainter.setFont( d->s_font );
            success = renderFloatItem( &pixmapPainter, viewport, layer );

            painter->drawPixmap( positivePosition( painter->viewport() ), d->m_cachePixmap );
        }
        else {
            painter->translate( positivePosition( painter->viewport() ) );

            painter->translate( 0.5, 0.5 );
            renderBackground( painter );
            painter->translate( d->s_padding, d->s_padding );

            painter->setFont( d->s_font );
            success = renderFloatItem( painter, viewport, layer );

            painter->resetTransform();
        }
    }
    else {
        success = renderOnMap( painter, viewport, renderPos, layer );
    }

    return success;
}

bool MarbleAbstractFloatItem::renderFloatItem( GeoPainter *painter,
					       ViewportParams *viewport,
					       GeoSceneLayer *layer )
{
    // In the derived method here is the right place to draw the
    // contents of the float item.

    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( layer );

    return true;
}

bool MarbleAbstractFloatItem::renderOnMap( GeoPainter     *painter,
					   ViewportParams *viewport,
					   const QString  &renderPos,
					   GeoSceneLayer  *layer )
{
    // In the derived method here is the place where you can draw some
    // additional stuff onto the map itself.

    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    return true;
}


bool MarbleAbstractFloatItem::eventFilter( QObject *object, QEvent *e )
{
    MarbleWidget *widget = dynamic_cast<MarbleWidget*>(object);
    if (!widget)
    {
        return false;
    }

    // Move float items
    bool cursorAboveFloatItem(false);
    if ( e->type() == QEvent::MouseMove
                || e->type() == QEvent::MouseButtonPress 
                || e->type() == QEvent::MouseButtonRelease ) {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);
        QRectF floatItemRect = QRectF(positivePosition(QRectF(0,0,widget->width(),widget->height())), size());

        // Click and move above a float item triggers moving the float item
        if (floatItemRect.contains(event->posF()))
        {
            cursorAboveFloatItem = true;

            if (e->type() == QEvent::MouseButtonPress && event->button() == Qt::LeftButton)
            {
                d->m_floatItemMoveStartPos = event->pos();
                return true;
            }

            if (e->type() == QEvent::MouseMove && event->buttons() & Qt::LeftButton)
            {
                const QPoint &point = event->pos();
                QPointF position = floatItemRect.topLeft();
                qreal newX = position.x()+point.x()-d->m_floatItemMoveStartPos.x();
                qreal newY = position.y()+point.y()-d->m_floatItemMoveStartPos.y();
                if (newX>=0 && newY>=0)
                {
                    setPosition(QPointF(newX,newY));
                    d->m_floatItemMoveStartPos = event->pos();
                    widget->setAttribute( Qt::WA_NoSystemBackground,  false );
                    QRegion dirtyRegion(floatItemRect.toRect());
                    dirtyRegion = dirtyRegion.united(QRect(newX,newY,size().width(),size().height()));
                    widget->repaint(dirtyRegion);
                    widget->setAttribute( Qt::WA_NoSystemBackground,  widget->map()->mapCoversViewport() );
                    return true;
                }
            }
        }

        // Adjusting Cursor shape
        if ( cursorAboveFloatItem )
        {
            widget->setCursor(QCursor(Qt::SizeAllCursor));
            return true;
        }
    }

    return false;
}

#include "MarbleAbstractFloatItem.moc"

