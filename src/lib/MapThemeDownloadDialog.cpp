//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2013 Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MapThemeDownloadDialog.h"
#include "ui_MapThemeDownloadDialog.h"

#include "MarbleDirs.h"
#include "NewstuffModel.h"

#include <QtGui/QPainter>
#include <QtGui/QTextDocument>
#include <QtGui/QAbstractTextDocumentLayout>
#include <QtGui/QStyledItemDelegate>

namespace Marble
{

class MapItemDelegate : public QStyledItemDelegate
{
public:
    MapItemDelegate( QListView* view );
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

private:
    QString text( const QModelIndex &index ) const;
    QListView* m_view;
};

class MapThemeDownloadDialog::Private : public Ui::MapThemeDownloadDialog
{
public:
    Private() :
        m_model()
    {}

    NewstuffModel m_model;
};

MapThemeDownloadDialog::MapThemeDownloadDialog( QWidget* parent ) :
    QDialog( parent ),
    d( new Private )
{
    d->setupUi( this );

    d->m_model.setTargetDirectory( MarbleDirs::localPath() + "/maps" );
    d->m_model.setProvider( "http://edu.kde.org/marble/newstuff/maps.xml" );

    d->listView->setIconSize( QSize( 130, 130 ) );
    d->listView->setAlternatingRowColors( true );
    d->listView->setItemDelegate( new MapItemDelegate( d->listView) );
    d->listView->setModel( &d->m_model );
}

MapThemeDownloadDialog::~MapThemeDownloadDialog()
{
    delete d;
}

MapItemDelegate::MapItemDelegate( QListView *view ) :
    m_view( view )
{
    // nothing to do
}

void MapItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 styleOption = option;
    styleOption.text = QString();
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &styleOption, painter);

    QAbstractTextDocumentLayout::PaintContext paintContext;
    if (styleOption.state & QStyle::State_Selected) {
        paintContext.palette.setColor(QPalette::Text,
            styleOption.palette.color(QPalette::Active, QPalette::HighlightedText));
    }

    // Draw the map preview icon
    QRect const rect = option.rect;
    QSize const iconSize = option.decorationSize;
    QRect const iconRect( rect.topLeft(), iconSize );
    QIcon const icon = qVariantValue<QIcon>( index.data( Qt::DecorationRole ) );
    painter->drawPixmap( iconRect, icon.pixmap( iconSize ) );

    // Draw summary, author, license info as text
    int const padding = 5;
    int const button_width = 50;
    QTextDocument document;
    document.setTextWidth( rect.width() - iconSize.width() - button_width - 4 * padding );
    document.setDefaultFont( option.font );
    document.setHtml( text( index ) );

    painter->save();
    painter->translate( iconRect.topRight() + QPoint( padding, 0 ) );
    painter->save();
    painter->setClipRect( 0, 0, document.textWidth(), rect.height() );
    document.documentLayout()->draw( painter, paintContext );
    painter->restore();

    // Draw buttons and installation progress
    //QApplication::style()->draw
    QStyleOptionButton buttonOption;
    buttonOption.state = option.state;
    buttonOption.state &= ~QStyle::State_HasFocus;

    buttonOption.palette = option.palette;
    buttonOption.features = QStyleOptionButton::None;
    buttonOption.text = tr( "Install" );
    QSize contentSize = buttonOption.fontMetrics.size( 0, buttonOption.text ) + QSize( 4, 4 );
    QSize buttonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton,
                                                                &buttonOption,
                                                                contentSize );
    buttonOption.rect.setTopLeft( QPoint( 0, 0 ) );
    buttonOption.rect.setSize( buttonSize );

    painter->translate( document.textWidth() + padding, padding );
    QApplication::style()->drawControl( QStyle::CE_PushButton, &buttonOption, painter );
    painter->restore();
}

QSize MapItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if ( index.column() == 0 ) {
        QSize const iconSize = option.decorationSize;
        QTextDocument doc;
        doc.setDefaultFont( option.font );
        doc.setTextWidth( m_view->width() - iconSize.width() - 50 - 15 );
        doc.setHtml( text( index ) );
        return QSize( iconSize.width() + doc.size().width(), iconSize.height() );
    }

    return QSize();
}

QString MapItemDelegate::text( const QModelIndex &index ) const
{
    //    Name = Qt::UserRole + 1,
    //    Author,
    //    License,
    //    Summary,
    //    Version,
    //    ReleaseDate,
    //    Preview,
    //    Payload,
    //    InstalledVersion,
    //    InstalledReleaseDate,
    //    IsInstalled,
    //    IsUpgradable,
    //    Category,
    //    IsTransitioning,
    //    PayloadSize

    QString const title = index.data().toString();
    QString const description = index.data( NewstuffModel::Summary ).toString();
    QString const license = index.data( NewstuffModel::License ).toString();
    QString const author = index.data( NewstuffModel::Author ).toString();
    return QString("<p><b>%1</b></p><p>%2</p><p>Author: %3<br />License: %4</p>").arg( title ).arg( description ).arg(author).arg(license);
}

}

#include "MapThemeDownloadDialog.moc"
