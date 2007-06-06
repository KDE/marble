//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "searchlistview.h"

#include <QtCore/QDebug>
#include <QtGui/QStandardItemModel>


SearchListView::SearchListView(QWidget* parent)
    : QListView(parent)
{
    connect(this, SIGNAL( activated( const QModelIndex& ) ),
            this, SIGNAL( centerOn ( const QModelIndex& ) ) );
}


void SearchListView::selectItem(const QString& text)
{
    if ( text.isEmpty() ) {
        clearSelection();
        scrollToTop();
        emit activated(QModelIndex());
        return;
    }

    QModelIndexList  resultlist;

    resultlist = model()->match( model()->index( 0, 0 ),
                                 Qt::DisplayRole,text, 1, 
                                 Qt::MatchStartsWith );

    if ( resultlist.size() > 0 ) {
        scrollToBottom(); 
        setCurrentIndex( resultlist[0] );
        scrollTo( resultlist[0] );
    }
    else
        qDebug( "noitem!" );
}


void SearchListView::activate()
{
    if ( selectedIndexes().size() > 0 )
        emit activated( currentIndex() );
}


#include "searchlistview.moc"
