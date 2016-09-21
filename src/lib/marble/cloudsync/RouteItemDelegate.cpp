#include "RouteItemDelegate.h"

#include "CloudRouteModel.h"

#ifdef MARBLE_NO_WEBKITWIDGETS
#include <QWidget>
#else
#include <QWebView>
#endif

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QListView>

namespace Marble {

RouteItemDelegate::RouteItemDelegate( QListView *view, CloudRouteModel *model ) :
    m_view( view ),
    m_model( model ),
    m_buttonWidth( 0 ),
    m_iconSize( 16 ),
    m_previewSize( 128 ),
    m_margin( 8 )
{
}

void RouteItemDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QStyleOptionViewItem styleOption = option;
    styleOption.text = QString();
    QApplication::style()->drawControl( QStyle::CE_ItemViewItem, &styleOption, painter );
    
    QAbstractTextDocumentLayout::PaintContext paintContext;
    if ( styleOption.state & QStyle::State_Selected)  {
        paintContext.palette.setColor( QPalette::Text, styleOption.palette.color( QPalette::Active, QPalette::HighlightedText ) );
    }

    QRect const iconRect = position( Preview, option );
    QIcon const icon = index.data( Qt::DecorationRole ).value<QIcon>();
    painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );

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

    bool cached = index.data( CloudRouteModel::IsCached ).toBool();
    bool downloading = index.data( CloudRouteModel::IsDownloading ).toBool();
    bool onCloud = index.data( CloudRouteModel::IsOnCloud ).toBool();

    if ( downloading ) {
        qint64 total = m_model->totalSize();
        qint64 progress = m_model->downloadedSize();

        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = position( Progressbar, option );
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = ( 100.0 * progress / total );
        progressBarOption.text = QString::number(progressBarOption.progress) + QLatin1Char('%');
        progressBarOption.textVisible = true;
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
    } else if ( !cached && onCloud ) {
        QStyleOptionButton downloadButton = button( DownloadButton, option );
        QRect downloadRect = position( DownloadButton, option );
        downloadButton.rect = downloadRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &downloadButton, painter );

        QStyleOptionButton cloudRemoveButton = button( RemoveFromCloudButton, option );
        QRect cloudRemoveRect = position( RemoveFromCloudButton, option );
        cloudRemoveButton.rect = cloudRemoveRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &cloudRemoveButton, painter );
    } else if ( cached && !onCloud ) {
        QStyleOptionButton openButton = button( OpenButton, option );
        QRect openRect = position( OpenButton, option );
        openButton.rect = openRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &openButton, painter );

        QStyleOptionButton cacheRemoveButton = button( RemoveFromCacheButton, option );
        QRect cacheRemoveRect = position( RemoveFromCacheButton, option );
        cacheRemoveButton.rect = cacheRemoveRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &cacheRemoveButton, painter );

        QStyleOptionButton uploadButton = button( UploadToCloudButton, option );
        QRect uploadRect = position( UploadToCloudButton, option );
        uploadButton.rect = uploadRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &uploadButton, painter );
    } else if ( cached && onCloud ) {
        QStyleOptionButton openButton = button( OpenButton, option );
        QRect openRect = position( OpenButton, option );
        openButton.rect = openRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &openButton, painter );

        QStyleOptionButton cacheRemoveButton = button( RemoveFromCacheButton, option );
        QRect cacheRemoveRect = position( RemoveFromCacheButton, option );
        cacheRemoveButton.rect = cacheRemoveRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &cacheRemoveButton, painter );
    }
}

QSize RouteItemDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    if ( index.column() == 0 ) {
        QTextDocument doc;
        doc.setDefaultFont( option.font );
        doc.setTextWidth( qMax( 128, m_view->contentsRect().width() - m_previewSize - buttonWidth( option ) ) );
        doc.setHtml( text( index ) );
        return QSize( qMax( 256, qRound( m_previewSize + buttonWidth( option ) + doc.size().width() + m_margin * 2 ) ),
                      qMax( m_previewSize + m_margin * 2, qRound( doc.size().height() ) ) );
    }

    return QSize();
}

bool RouteItemDelegate::editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index )
{
    Q_UNUSED( model );

    if ( ( event->type() == QEvent::MouseButtonRelease ) ) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );
        QPoint pos = mouseEvent->pos();
        
        bool cached = index.data( CloudRouteModel::IsCached ).toBool();
        bool onCloud = index.data( CloudRouteModel::IsOnCloud ).toBool();

        if( cached && !onCloud ) {
            QRect uploadRect = position( UploadToCloudButton, option );

            if ( uploadRect.contains( pos ) ) {
                QString timestamp = index.data( CloudRouteModel::Timestamp ).toString();
                emit uploadToCloudButtonClicked( timestamp );
                return true;
            }
        }

        if ( cached ) {
            QRect openRect = position( OpenButton, option );
            QRect cacheRemoveRect = position( RemoveFromCacheButton, option );

            if ( openRect.contains( pos ) ) {
                QString timestamp = index.data( CloudRouteModel::Timestamp ).toString();
                emit openButtonClicked( timestamp );
                return true;
            } else if ( cacheRemoveRect.contains( pos ) ) {
                QString timestamp = index.data( CloudRouteModel::Timestamp ).toString();
                emit removeFromCacheButtonClicked( timestamp );
                return true;
            }
        } else {
            QRect downloadRect = position( DownloadButton, option );
            QRect cloudRemoveRect = position( RemoveFromCloudButton, option );
            
            if ( downloadRect.contains( pos ) ) {
                QString timestamp = index.data( CloudRouteModel::Timestamp ).toString();
                m_model->setDownloadingItem( index );
                emit downloadButtonClicked( timestamp );
                return true;
            }
            
            if ( cloudRemoveRect.contains( pos ) ) {
                QString timestamp = index.data( CloudRouteModel::Timestamp ).toString();
                emit deleteButtonClicked( timestamp );
                return true;
            }
        }
    }
    
    return false;
}

int RouteItemDelegate::buttonWidth( const QStyleOptionViewItem &option ) const
{
    if ( m_buttonWidth <= 0 ) {
        int const openWidth = option.fontMetrics.size( 0, tr( "Open" ) ).width();
        int const downloadWidth = option.fontMetrics.size( 0, tr( "Load" ) ).width();
        int const cacheWidth = option.fontMetrics.size( 0, tr( "Remove from device" ) ).width();
        int const cloudWidth = option.fontMetrics.size( 0, tr( "Delete from cloud" ) ).width();
        m_buttonWidth = 2 * m_iconSize + qMax( qMax( openWidth, downloadWidth ), qMax( cacheWidth, cloudWidth ) );
    }

    return m_buttonWidth;
}

QStyleOptionButton RouteItemDelegate::button( Element element, const QStyleOptionViewItem &option ) const
{
    QStyleOptionButton result;
    result.state = option.state;
    result.state &= ~QStyle::State_HasFocus;

    result.palette = option.palette;
    result.features = QStyleOptionButton::None;

    switch ( element ) {
    case OpenButton:
        result.text = tr( "Open" );
        result.icon = QIcon(QStringLiteral(":/marble/document-open.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    case DownloadButton:
        result.text = tr( "Load" );
        result.icon = QIcon(QStringLiteral(":/marble/dialog-ok.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    case RemoveFromCacheButton:
        result.text = tr( "Remove from device" );
        result.icon = QIcon(QStringLiteral(":/marble/edit-clear.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    case RemoveFromCloudButton:
        result.text = tr( "Delete from cloud" );
        result.icon = QIcon(QStringLiteral(":/marble/edit-delete.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    case UploadToCloudButton:
        result.text = tr( "Upload to cloud" );
        result.icon = QIcon(QStringLiteral(":/icons/cloud-upload.png"));
        result.iconSize = QSize( m_iconSize, m_iconSize );
        break;
    default:
        // Ignored.
        break;
    }

    return result;
}

QString RouteItemDelegate::text( const QModelIndex& index )
{
    return QString( "%0" ).arg( index.data( CloudRouteModel::Name ).toString() );
    // TODO: Show distance and duration
    //return QString( "%0<br /><b>Duration:</b> %1<br/><b>Distance:</b> %2" )
            //.arg( index.data( CloudRouteModel::Name ).toString() )
            //.arg( index.data( CloudRouteModel::Duration ).toString() )
            //.arg( index.data( CloudRouteModel::Distance ).toString() );
}

QRect RouteItemDelegate::position( Element element, const QStyleOptionViewItem& option ) const
{   
    int const width = buttonWidth( option );
    QPoint const firstColumn = option.rect.topLeft() + QPoint( m_margin, m_margin );
    QPoint const secondColumn = firstColumn + QPoint( m_previewSize + m_margin, 0 );
    QPoint const thirdColumn = QPoint( option.rect.width() - width - option.decorationSize.width(), firstColumn.y() );

    switch (element) {
    case Text:
        return QRect( secondColumn, QSize( thirdColumn.x() - secondColumn.x(), option.rect.height() ) );
    case OpenButton:
    case DownloadButton:
    {
        QStyleOptionButton optionButton = button( element, option );
        QSize size = option.fontMetrics.size( 0, optionButton.text ) + QSize( 4, 4 );
        QSize buttonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &optionButton, size );
        buttonSize.setWidth( width );
        return QRect( thirdColumn, buttonSize );
    }
    case RemoveFromCacheButton:
    case RemoveFromCloudButton:
    {
        QStyleOptionButton optionButton = button( element, option );
        QSize size = option.fontMetrics.size( 0, optionButton.text ) + QSize( 4, 4 );
        QSize buttonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &optionButton, size );
        buttonSize.setWidth( width );
        return QRect( thirdColumn + QPoint( 0, buttonSize.height() ), buttonSize );
    }
    case Progressbar:
    {
        QSize const progressSize = QSize( width, option.fontMetrics.height() + 4 );
        return QRect( thirdColumn + QPoint( 0, 10 ), progressSize );
    }
    case Preview:
    {
        return QRect( firstColumn, QSize( m_previewSize, m_previewSize) );
    }
    case UploadToCloudButton:
    {
        QStyleOptionButton optionButton = button( element, option );
        QSize size = option.fontMetrics.size( 0, optionButton.text ) + QSize( 4, 4 );
        QSize buttonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &optionButton, size );
        buttonSize.setWidth( width );
        return QRect( thirdColumn + QPoint( 0, buttonSize.height() * 2 ), buttonSize );
    }
    }
    
    return QRect();
}

}

#include "moc_RouteItemDelegate.cpp"
