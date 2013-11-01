#ifndef ROUTEITEMDELEGATE_H
#define ROUTEITEMDELEGATE_H

#include "CloudRouteModel.h"

#include <QListView>
#include <MarbleWidget.h>
#include <QStyledItemDelegate>

namespace Marble {

class RouteItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    RouteItemDelegate( QListView *view, CloudRouteModel *model );

    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);


signals:
    void downloadButtonClicked( QString timestamp );
    void openButtonClicked( QString timestamp );
    void deleteButtonClicked( QString timestamp );
    void removeFromCacheButtonClicked( QString timestamp );
    void uploadToCloudButtonClicked( QString timestamp );

private:
    enum Element {
        Text,
        OpenButton,
        DownloadButton,
        RemoveFromCacheButton,
        RemoveFromCloudButton,
        Progressbar,
        Preview,
        UploadToCloudButton
    };

    int buttonWidth( const QStyleOptionViewItem &option ) const;
    QStyleOptionButton button( Element element, const QStyleOptionViewItem &option ) const;
    QString text( const QModelIndex &index ) const;
    QRect position( Element element, const QStyleOptionViewItem &option ) const;

    QListView *m_view;
    CloudRouteModel *m_model;
    mutable int m_buttonWidth;
    int const m_iconSize;
    int const m_previewSize;
    int const m_margin;
};

}

#endif
