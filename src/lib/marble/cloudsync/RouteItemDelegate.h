#ifndef ROUTEITEMDELEGATE_H
#define ROUTEITEMDELEGATE_H

#include <QStyledItemDelegate>

class QListView;

namespace Marble {
class CloudRouteModel;

class RouteItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    RouteItemDelegate( QListView *view, CloudRouteModel *model );

    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);


Q_SIGNALS:
    void downloadButtonClicked( const QString& timestamp );
    void openButtonClicked( const QString& timestamp );
    void deleteButtonClicked( const QString& timestamp );
    void removeFromCacheButtonClicked( const QString& timestamp );
    void uploadToCloudButtonClicked( const QString& timestamp );

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
    static QString text( const QModelIndex &index );
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
