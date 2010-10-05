//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#include "MarbleThemeSelectView.h"

#include "MarbleDebug.h"
#include <QtGui/QResizeEvent>

using namespace Marble;

MarbleThemeSelectView::MarbleThemeSelectView(QWidget *parent)
    : QListView( parent ),
      d( 0 )                    // No private data yet.
{
    setViewMode( QListView::IconMode );
    setFlow( QListView::LeftToRight );
    setWrapping( true ); 
    setMovement( QListView::Static );
    setResizeMode( QListView::Fixed );
    setUniformItemSizes ( true );
    setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    setEditTriggers( QAbstractItemView::NoEditTriggers );
    setIconSize( QSize( 136,136 ) );
	setSelectionMode( QAbstractItemView::SingleSelection );

    connect( this, SIGNAL( pressed( QModelIndex ) ),
                   SLOT( selectedMapTheme( QModelIndex ) ) );
}


void MarbleThemeSelectView::resizeEvent(QResizeEvent* event)
{
    QListView::resizeEvent(event);

    QSize  size = gridSize();
    size.setWidth(event->size().width());
    setGridSize(size);
}


void MarbleThemeSelectView::selectedMapTheme( QModelIndex index )
{
    const QAbstractItemModel  *model = index.model();

    QModelIndex  colindex        = model->index( index.row(), 1, 
                                                 QModelIndex() );
    QString      currentmaptheme = (model->data( colindex )).toString();
    mDebug() << currentmaptheme;
    emit selectMapTheme( currentmaptheme ); 
}


#include "MarbleThemeSelectView.moc"
