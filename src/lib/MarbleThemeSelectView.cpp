//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "MarbleThemeSelectView.h"

#include <QtGui/QResizeEvent>


MarbleThemeSelectView::MarbleThemeSelectView(QWidget *parent)
    : QListView( parent ),
      d( 0 )                    // No private data yet.
{
    setViewMode( QListView::IconMode );
    setFlow( QListView::TopToBottom ); 
    setWrapping( false ); 
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

#if 0
MarbleThemeSelectView::setModel( QAbstractItemModel * model )
{
}
#endif

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
    emit selectMapTheme( currentmaptheme ); 

    // qDebug() << currentmaptheme;
}


#include "MarbleThemeSelectView.moc"
