//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "SearchInputWidget.h"

#include "MarblePlacemarkModel.h"

#include <QtGui/QCompleter>

namespace Marble {

SearchInputWidget::SearchInputWidget( QWidget *parent ) :
    MarbleLineEdit( parent ),
    m_completer( new QCompleter( this ) )
{
#if QT_VERSION >= 0x40700
    setPlaceholderText( tr( "Search" ) );
#endif
    QPixmap const decorator = QPixmap( ":/icons/16x16/edit-find.png" );
    Q_ASSERT( !decorator.isNull() );
    setDecorator( decorator );

    connect( this, SIGNAL( clearButtonClicked() ), this, SLOT( search() ) );
    connect( this, SIGNAL( returnPressed() ), this, SLOT( search() ) );
    connect( this, SIGNAL( decoratorButtonClicked() ), this, SLOT( search() ) );

    m_sortFilter.setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    m_sortFilter.sort( 0, Qt::DescendingOrder );
    m_sortFilter.setDynamicSortFilter( true );

    m_completer->setCompletionRole( Qt::DisplayRole );
    m_completer->setCaseSensitivity( Qt::CaseInsensitive );
    m_completer->setModel( &m_sortFilter );
    setCompleter( m_completer );
    connect( m_completer, SIGNAL( activated( QModelIndex ) ), this, SLOT( centerOnSearchSuggestion( QModelIndex ) ) );
}

void SearchInputWidget::setCompletionModel( QAbstractItemModel *completionModel )
{
    m_sortFilter.setSourceModel( completionModel );
}

void SearchInputWidget::search()
{
    QString const searchTerm = text();
    if ( !searchTerm.isEmpty() ) {
        setBusy( true );
    }
    emit search( searchTerm );
}

void SearchInputWidget::disableSearchAnimation()
{
    setBusy( false );
}

void SearchInputWidget::centerOnSearchSuggestion(const QModelIndex &index )
{
    QAbstractItemModel const * model = completer()->completionModel();
    QVariant const value = model->data( index, MarblePlacemarkModel::CoordinateRole );
    GeoDataCoordinates const coordinates = qVariantValue<GeoDataCoordinates>( value );
    emit centerOn( coordinates );
}

}

#include "SearchInputWidget.moc"
