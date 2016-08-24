//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2013 Dennis Nienhüser <nienhueser@kde.org>
//

#include "MapThemeDownloadDialog.h"
#include "ui_MapThemeDownloadDialog.h"

#include "MarbleDirs.h"
#include "NewstuffModel.h"
#include "MarbleWidget.h"

#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QStyledItemDelegate>

namespace Marble
{

class MapItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MapItemDelegate( QListView* view, NewstuffModel* newstuffModel, MarbleWidget* marbleWidget );
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

protected:
    bool editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index );

private:
    enum Element {
        Icon,
        Text,
        InstallButton,
        UpgradeButton,
        OpenButton,
        CancelButton,
        RemoveButton,
        ProgressReport
    };

    int buttonWidth( const QStyleOptionViewItem &option ) const;
    QStyleOptionButton button( Element element, const QStyleOptionViewItem &option ) const;
    QRect position( Element element, const QStyleOptionViewItem &option ) const;
    static QString text( const QModelIndex &index );
    QListView* m_view;
    NewstuffModel* m_newstuffModel;
    mutable int m_buttonWidth;
    int const m_margin;
    int const m_iconSize;
    MarbleWidget* m_marbleWidget;
};

class Q_DECL_HIDDEN MapThemeDownloadDialog::Private : public Ui::MapThemeDownloadDialog
{
public:
    Private() :
        m_model()
    {}

    NewstuffModel m_model;
};

MapThemeDownloadDialog::MapThemeDownloadDialog( MarbleWidget* marbleWidget ) :
    QDialog( marbleWidget ),
    d( new Private )
{
    d->setupUi( this );

    d->m_model.setTargetDirectory(MarbleDirs::localPath() + QLatin1String("/maps"));
    d->m_model.setProvider( "https://marble.kde.org/maps-v3.xml" );
    d->m_model.setRegistryFile(MarbleDirs::localPath() + QLatin1String("/newstuff/marble-map-themes.knsregistry"), Marble::NewstuffModel::NameTag);

    d->listView->setIconSize( QSize( 130, 130 ) );
    d->listView->setAlternatingRowColors( true );
    d->listView->setUniformItemSizes( false );
    d->listView->setResizeMode( QListView::Adjust );
    d->listView->setItemDelegate( new MapItemDelegate( d->listView, &d->m_model, marbleWidget ) );
    d->listView->setModel( &d->m_model );
}

MapThemeDownloadDialog::~MapThemeDownloadDialog()
{
    delete d;
}

MapItemDelegate::MapItemDelegate( QListView *view , NewstuffModel *newstuffModel, MarbleWidget* marbleWidget ) :
    m_view( view ),
    m_newstuffModel( newstuffModel ),
    m_buttonWidth( 0 ),
    m_margin( 5 ),
    m_iconSize( 16 ),
    m_marbleWidget( marbleWidget )
{
    // nothing to do
}

void MapItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QStyleOptionViewItem styleOption = option;
    styleOption.text = QString();
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &styleOption, painter);

    QAbstractTextDocumentLayout::PaintContext paintContext;
    if (styleOption.state & QStyle::State_Selected) {
        paintContext.palette.setColor(QPalette::Text,
            styleOption.palette.color(QPalette::Active, QPalette::HighlightedText));
    }

    // Draw the map preview icon
    QRect const iconRect = position( Icon, option );
    QIcon const icon = index.data( Qt::DecorationRole ).value<QIcon>();
    painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );

    // Draw summary, author, and similar information
    QTextDocument document;
    QRect const textRect = position( Text, option );
    document.setTextWidth( textRect.width() );
    document.setDefaultFont( option.font );
    document.setHtml( text( index ) );

    painter->save();
    painter->translate( textRect.topLeft() );
    painter->setClipRect( 0, 0, textRect.width(), textRect.height() );
    document.documentLayout()->draw( painter, paintContext );
    painter->restore();

    // Draw buttons and installation progress
    if ( index.data( NewstuffModel::IsTransitioning ).toBool() ) {
        qint64 total = qMax( qint64( 1 ), index.data( NewstuffModel::PayloadSize ).value<qint64>() );
        qint64 progress = index.data( NewstuffModel::DownloadedSize ).value<qint64>();

        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = position( ProgressReport, option );
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = ( 100.0 * progress / total );
        progressBarOption.text = QString::number(progressBarOption.progress) + QLatin1Char('%');
        progressBarOption.textVisible = true;
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);

        QStyleOptionButton cancelButton = button( CancelButton, option );
        QRect installRect = position( CancelButton, option );
        cancelButton.rect = installRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &cancelButton, painter );
        QRect buttonTextRect(installRect);
        buttonTextRect.adjust(cancelButton.iconSize.width() + 4, 0, 0, 0);
        painter->drawText(buttonTextRect, Qt::AlignCenter, cancelButton.text);
    } else {
        bool const installed = index.data( NewstuffModel::IsInstalled ).toBool();
        bool const upgradable = index.data( NewstuffModel::IsUpgradable ).toBool();
        Element element = InstallButton;
        if ( installed ) {
            element = upgradable ? UpgradeButton : OpenButton;
        }
        QStyleOptionButton actionButton = button( element, option );
        QRect installRect = position( element, option );
        actionButton.rect = installRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &actionButton, painter );
        QRect buttonTextRect(installRect);
        buttonTextRect.adjust(actionButton.iconSize.width() + 4, 0, 0, 0);
        painter->drawText(buttonTextRect, Qt::AlignCenter, actionButton.text);

        if ( installed ) {
            QStyleOptionButton removeButton = button( RemoveButton, option );
            QRect removeRect = position( RemoveButton, option );
            removeButton.rect = removeRect;
            QApplication::style()->drawControl( QStyle::CE_PushButton, &removeButton, painter );
            buttonTextRect = removeRect;
            buttonTextRect.adjust(removeButton.iconSize.width() + 4, 0, 0 ,0);
            painter->drawText(buttonTextRect, Qt::AlignCenter, removeButton.text);
        }
    }
}

QSize MapItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if ( index.column() == 0 ) {
        QSize const iconSize = option.decorationSize;
        QTextDocument doc;
        doc.setDefaultFont( option.font );
        doc.setTextWidth( qMax( 200, m_view->contentsRect().width() - iconSize.width() - buttonWidth( option ) - 3 * m_margin ) );
        doc.setHtml( text( index ) );
        return QSize( iconSize.width() + doc.size().width() + buttonWidth( option ) + 3 * m_margin,
                      2 + qMax( iconSize.height(), qRound( doc.size().height() ) ) );
    }

    return QSize();
}

bool MapItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if ( ( event->type() == QEvent::MouseButtonRelease ) ) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );
        if ( index.data( NewstuffModel::IsTransitioning ).toBool() ) {
            QRect cancelRect = position( CancelButton, option );
            if ( cancelRect.contains( mouseEvent->pos() ) ) {
                m_newstuffModel->cancel( index.row() );
                return true;
            }
        } else {
            bool const installed = index.data( NewstuffModel::IsInstalled ).toBool();
            bool const upgradable = index.data( NewstuffModel::IsUpgradable ).toBool();

            if ( !installed || upgradable ) {
                QRect installRect = position( InstallButton, option );
                if ( installRect.contains( mouseEvent->pos() ) ) {
                    m_newstuffModel->install( index.row() );
                    return true;
                }
            }

            if ( installed && !upgradable && m_marbleWidget ) {
                QRect openRect = position( OpenButton, option );
                if ( openRect.contains( mouseEvent->pos() ) ) {
                    QStringList const files = index.data( NewstuffModel::InstalledFiles ).toStringList();
                    foreach( const QString &file, files ) {
                        if ( file.endsWith( QLatin1String( ".dgml" ) ) ) {
                            QFileInfo dgmlFile( file );
                            QDir baseDir = dgmlFile.dir();
                            baseDir.cdUp();
                            baseDir.cdUp();
                            int const index = baseDir.absolutePath().size();
                            QString const mapTheme = dgmlFile.absoluteFilePath().mid( index+1 );
                            m_marbleWidget->setMapThemeId( mapTheme );
                            return true;
                        }
                    }
                }
            }

            if ( installed ) {
                QRect removeRect = position( RemoveButton, option );
                if ( removeRect.contains( mouseEvent->pos() ) ) {
                    m_newstuffModel->uninstall( index.row() );
                    return true;
                }
            }
        }
    }

    return false;
}

int MapItemDelegate::buttonWidth(const QStyleOptionViewItem &option) const
{
    if ( m_buttonWidth <= 0 ) {
        int const installWidth = option.fontMetrics.size( 0, tr( "Install" ) ).width();
        int const removeWidth = option.fontMetrics.size( 0, tr( "Remove" ) ).width();
        int const cancelWidth = option.fontMetrics.size( 0, tr( "Cancel" ) ).width();
        int const upgradeWidth = option.fontMetrics.size( 0, tr( "Upgrade" ) ).width();
        m_buttonWidth = 2 * m_iconSize + qMax( qMax( installWidth, removeWidth ),
                                           qMax( cancelWidth, upgradeWidth ) );
    }

    return m_buttonWidth;
}

QStyleOptionButton MapItemDelegate::button( Element element, const QStyleOptionViewItem &option ) const
{
    QStyleOptionButton result;
    result.state = option.state;
    result.state &= ~QStyle::State_HasFocus;

    result.palette = option.palette;
    result.features = QStyleOptionButton::None;

    switch (element) {
    case InstallButton:
        result.text = tr( "Install" );
        result.icon = QIcon(QStringLiteral(":/marble/dialog-ok.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    case UpgradeButton:
        result.text = tr( "Update" );
        result.icon = QIcon(QStringLiteral(":/marble/system-software-update.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    case OpenButton:
        result.text = tr( "Open" );
        result.icon = QIcon(QStringLiteral(":/marble/document-open.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    case CancelButton:
        result.text = tr( "Cancel" );
        break;
    case RemoveButton:
        result.text = tr( "Remove" );
        result.icon = QIcon(QStringLiteral(":/marble/edit-delete.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    default:
        // ignored
        break;
    }

    return result;
}

QRect MapItemDelegate::position(Element element, const QStyleOptionViewItem &option ) const
{
    int const width = buttonWidth( option );
    QPoint const topLeftCol1 = option.rect.topLeft() + QPoint( 0, 2 );
    QPoint const topLeftCol2 = topLeftCol1 + QPoint( option.decorationSize.width(), 0 );
    QPoint const topLeftCol3 = topLeftCol2 + QPoint( option.rect.width() - 3 * m_margin - width - option.decorationSize.width(), 0 );
    switch (element) {
    case Icon:
        return QRect( topLeftCol1, option.decorationSize );
    case Text:
        return QRect( topLeftCol2, QSize( topLeftCol3.x()-topLeftCol2.x(), option.rect.height() ) );
    case InstallButton:
    case UpgradeButton:
    case OpenButton:
    {
        QStyleOptionButton optionButton = button( element, option );
        QSize size = option.fontMetrics.size( 0, optionButton.text ) + QSize( 4, 4 );
        QSize buttonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &optionButton, size );
        buttonSize.setWidth( width );
        return QRect( topLeftCol3, buttonSize );
    }
    case RemoveButton:
    case CancelButton:
    {
        QStyleOptionButton optionButton = button( element, option );
        QSize size = option.fontMetrics.size( 0, optionButton.text ) + QSize( 4, 4 );
        QSize buttonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &optionButton, size );
        buttonSize.setWidth( width );
        return QRect( topLeftCol3 + QPoint( 0, option.fontMetrics.height() + 8 + m_margin ), buttonSize );
    }
    case ProgressReport:
    {
        QSize const progressSize = QSize( width, option.fontMetrics.height() + 4 );
        return QRect( topLeftCol3 + QPoint( 0, m_margin ), progressSize );
    }
    }

    Q_ASSERT(false);
    return QRect();
}

QString MapItemDelegate::text( const QModelIndex &index )
{
    qreal const size = index.data( NewstuffModel::PayloadSize ).toLongLong() / 1024.0 / 1024.0;
    // Fields are typically not longer than 200 characters. Prevent excessive long text here anyway
    // due to bug 319542
    int const maxEntrySize = 4096;
    return QString("<p><b>%1</b><br />%2</p><p>Author: %3<br />License: %4<br />Version %5 (%6) %7</p>")
            .arg( index.data().toString() )
            .arg( index.data( NewstuffModel::Summary ).toString().left( maxEntrySize ) )
            .arg( index.data( NewstuffModel::Author ).toString().left( maxEntrySize ) )
            .arg( index.data( NewstuffModel::License ).toString().left( maxEntrySize ) )
            .arg( index.data( NewstuffModel::Version ).toString().left( maxEntrySize ) )
            .arg( index.data( NewstuffModel::ReleaseDate ).toString().left( maxEntrySize ) )
            .arg( size > 0 ? QString( "%1 MB" ).arg( size, 0, 'f', 1 ) : QString() );
}

}

#include "MapThemeDownloadDialog.moc" // needed for Q_OBJECT here in source
