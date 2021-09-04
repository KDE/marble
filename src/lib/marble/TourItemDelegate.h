// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// SPDX-FileCopyrightText: 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef TOURITEMDELEGATE_H
#define TOURITEMDELEGATE_H

#include <QStyledItemDelegate>

class QListView;

namespace Marble
{

class MarbleWidget;
class GeoDataAnimatedUpdate;
class GeoDataPlaylist;
class GeoDataFeature;
class TourWidget;

class TourItemDelegate : public QStyledItemDelegate
{
Q_OBJECT

public:
    TourItemDelegate( QListView* view, MarbleWidget* widget, TourWidget* tour );
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
    QWidget* createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const override;
    bool editable() const;
    void setEditable( bool editable );
    QModelIndex firstFlyTo() const;
    bool editAnimatedUpdate(GeoDataAnimatedUpdate *animatedUpdate, bool create = true );
    QString defaultFeatureId() const;
    GeoDataFeature *findFeature( const QString &id ) const;

public Q_SLOTS:
    /** Editing duration for first flyTo element in playlist will be disabled.  */
    void setFirstFlyTo( const QPersistentModelIndex &index );
    /** Sets id of default feature for Remove Item */
    void setDefaultFeatureId( const QString &id );

Q_SIGNALS:
    void editingChanged( const QModelIndex& index );
    void edited( const QModelIndex& index );
    void editableChanged( bool editable );
    void firstFlyToChanged( const QPersistentModelIndex &newFirstFlyTo );
    void featureIdsChanged( const QStringList& ids );
    void defaultFeatureIdChanged( const QString& id );

public:

    enum Element {
        GeoDataElementIcon,
        Label,
        EditButton,
        ActionButton
    };

protected:
    bool editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index ) override;

private Q_SLOTS:
    void closeEditor(const QModelIndex& index);

private:
    static QRect position( Element element, const QStyleOptionViewItem &option );
    static QStringList findIds(const GeoDataPlaylist &playlist, bool onlyFeatures = false);
    GeoDataPlaylist *playlist() const;
    QList<QPersistentModelIndex> m_editingIndices;
    QListView* m_listView;
    MarbleWidget *m_widget;
    bool m_editable;
    QPersistentModelIndex m_firstFlyTo;
    QString m_defaultFeatureId;
    TourWidget* m_tourWidget;
};

} // namespace Marble

#endif
