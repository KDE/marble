//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010,2011  Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "MarbleGLView.h"
#include "MarbleGLView.moc"

#include <cmath>

#include <QtCore/QTime>
#include <QtGui/QPaintEvent>

#include "AbstractProjection.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "MarbleHeight.h"
#include "MarbleMap.h"
#include "TileId.h"
#include "ViewParams.h"
#include "ViewportParams.h"

namespace Marble
{

class MarbleGLView::Private
{
 public:
    Private( MarbleMap *map, MarbleGLView *widget )
        : m_widget( widget ),
          m_map( map )
    {
        // Widget settings
        m_widget->setFocusPolicy( Qt::WheelFocus );
        m_widget->setFocus( Qt::OtherFocusReason );
#if QT_VERSION >= 0x40600
        m_widget->setAttribute( Qt::WA_AcceptTouchEvents );
#endif

        // Set background: black.
        m_widget->setPalette( QPalette ( Qt::black ) );

        // Set whether the black space gets displayed or the earth gets simply 
        // displayed on the widget background.
        m_widget->setAutoFillBackground( true );

        m_widget->setMouseTracking( m_widget );
    }

    ~Private()
    {
    }

    MarbleGLView *const m_widget;
    MarbleMap    *const m_map;
};


MarbleGLView::MarbleGLView( MarbleMap *map, QWidget *parent )
    : QGLWidget( parent ),
      d( new MarbleGLView::Private( map, this ) )
{
//    setAttribute( Qt::WA_PaintOnScreen, true );
    setAutoFillBackground( false );
    setAutoBufferSwap( true );
}

MarbleGLView::~MarbleGLView()
{
    delete d;
}

// ----------------------------------------------------------------


void MarbleGLView::resizeGL( int width, int height )
{
    d->m_map->setSize( width, height );
}

void MarbleGLView::initializeGL()
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glEnable( GL_TEXTURE_2D );
}

void MarbleGLView::paintEvent( QPaintEvent *evt )
{
    makeCurrent();

    bool  doClip = true;
    if ( d->m_map->projection() == Spherical )
        doClip = ( d->m_map->radius() > width() / 2
                   || d->m_map->radius() > height() / 2 );

    // Create a painter that will do the painting.
    GeoPainter painter( this, d->m_map->viewport(),
                        d->m_map->mapQuality(), doClip );
    QRect  dirtyRect = evt->rect();

    painter.beginNativePainting();

    d->m_map->paint( painter, dirtyRect );

    painter.endNativePainting();
}

}
