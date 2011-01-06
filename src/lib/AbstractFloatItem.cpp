//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//

// Self
#include "AbstractFloatItem.h"

// Qt
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QDialog>

// Marble
#include "MarbleDebug.h"

namespace Marble
{

class AbstractFloatItemPrivate
{
  public:
    AbstractFloatItemPrivate()
    {
    }

    ~AbstractFloatItemPrivate()
    {
    }


    static QPen         s_pen;
    static QFont        s_font;
};

QPen         AbstractFloatItemPrivate::s_pen = QPen( Qt::black );
#ifdef Q_OS_MACX
    QFont AbstractFloatItemPrivate::s_font = QFont( "Sans Serif", 10 );
#else
    QFont AbstractFloatItemPrivate::s_font = QFont( "Sans Serif", 8 );
#endif

AbstractFloatItem::AbstractFloatItem( const QPointF &point, const QSizeF &size )
    : RenderPlugin(),
      FrameGraphicsItem(),
      d( new AbstractFloatItemPrivate() )
{
    setCacheMode( MarbleGraphicsItem::ItemCoordinateCache );
    setFrame( RectFrame );
    setPadding( 4.0 );
    setContentSize( size );
    setPosition( point );
}

AbstractFloatItem::~AbstractFloatItem()
{
    delete d;
}

QPen AbstractFloatItem::pen() const
{
    return d->s_pen;
}

void AbstractFloatItem::setPen( const QPen &pen )
{
    d->s_pen = pen;
    update();
}

QFont AbstractFloatItem::font() const
{
    return d->s_font;
}

void AbstractFloatItem::setFont( const QFont &font )
{
    d->s_font = font;
    update();
}

QString AbstractFloatItem::renderPolicy() const
{
    return "ALWAYS";
}

QStringList AbstractFloatItem::renderPosition() const
{
    return QStringList( "FLOAT_ITEM" );
}

void AbstractFloatItem::setVisible( bool visible )
{
    // Reimplemented since AbstractFloatItem does multiple inheritance 
    // and the (set)Visible() methods are available in both base classes!
    RenderPlugin::setVisible( visible );
}

bool AbstractFloatItem::visible() const
{
    // Reimplemented since AbstractFloatItem does multiple inheritance 
    // and the (set)Visible() methods are available in both base classes!
    return RenderPlugin::visible();
}

void AbstractFloatItem::setPositionLocked( bool lock )
{
    ScreenGraphicsItem::GraphicsItemFlags flags = this->flags();

    if ( lock ) {
        flags &= ~ScreenGraphicsItem::ItemIsMovable;
    }
    else {
        flags |= ScreenGraphicsItem::ItemIsMovable;
    }

    setFlags( flags );
}

bool AbstractFloatItem::positionLocked()
{
    return ( flags() & ScreenGraphicsItem::ItemIsMovable ) ? false : true;
}

bool AbstractFloatItem::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    if( e->type() == QEvent::ContextMenu )
    {
        QWidget *widget = dynamic_cast<QWidget *>( object );
        QContextMenuEvent *menuEvent = dynamic_cast<QContextMenuEvent *> ( e );
        if( widget != NULL && menuEvent != NULL && contains( menuEvent->pos() ) )
        {
            contextMenuEvent( widget, menuEvent );
            return true;
        }
        return false;
    }
    else if( e->type() == QEvent::ToolTip )
    {
        QHelpEvent *helpEvent = dynamic_cast<QHelpEvent *>( e );
        if( helpEvent != NULL && contains( helpEvent->pos() ) )
        {
            toolTipEvent( helpEvent );
            return true;
        }
        return false;
    }
    else
        return ScreenGraphicsItem::eventFilter( object, e );
}

void AbstractFloatItem::contextMenuEvent ( QWidget *w, QContextMenuEvent *e )
{
    QMenu menu;
    QAction *lockaction = menu.addAction( tr( "&Lock" ) );
    lockaction->setCheckable( true );
    lockaction->setChecked( positionLocked() );
    connect( lockaction, SIGNAL( triggered( bool ) ),
             this, SLOT( setPositionLocked( bool ) ) );
    QAction *hideaction = menu.addAction( tr( "&Hide" ) );
    connect( hideaction, SIGNAL( triggered() ),
             this, SLOT( hide() ) );
    QDialog *dialog = configDialog();
    if( dialog != NULL )
    {
        menu.addSeparator();
        QAction *configaction = menu.addAction( tr( "&Configure..." ) );
        connect( configaction, SIGNAL( triggered() ),
                 dialog, SLOT( exec() ) );
    }
    menu.exec( w->mapToGlobal( e->pos() ) );
}

void AbstractFloatItem::toolTipEvent ( QHelpEvent *e )
{
    Q_UNUSED( e );
}

bool AbstractFloatItem::render( GeoPainter *painter, ViewportParams *viewport,
             const QString& renderPos, GeoSceneLayer * layer )
{
    if ( !enabled() || !visible() ) {
        return true;
    }

    if ( renderPos == "FLOAT_ITEM" ) {
        paintEvent( painter, viewport, renderPos, layer );
        return true;
    }
    else {
        return renderOnMap( painter, viewport, renderPos, layer );
    }
}

bool AbstractFloatItem::renderOnMap( GeoPainter     *painter,
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

void AbstractFloatItem::show()
{
    setVisible( true );
}

void AbstractFloatItem::hide()
{
    setVisible( false );
}

}

#include "AbstractFloatItem.moc"
