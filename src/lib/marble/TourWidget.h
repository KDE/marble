//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#ifndef MARBLE_TOURWIDGET_H
#define MARBLE_TOURWIDGET_H

#include "marble_export.h"

#include <QWidget>
#include <QStyledItemDelegate>
#include <QListView>
#include <GeoDataCoordinates.h>

class QItemSelection;
class QModelIndex;

namespace Marble
{

class FileManager;
class GeoDataPlacemark;
class GeoDataLatLonBox;
class GeoDataTreeModel;
class GeoDataFeature;
class MarbleWidget;

class TourWidgetPrivate;

class MARBLE_EXPORT TourWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit TourWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~TourWidget();

    void setMarbleWidget( MarbleWidget *widget );
    bool openTour( const QString &filename );

public Q_SLOTS:
    void startPlaying();
    void pausePlaying();
    void stopPlaying();

Q_SIGNALS:
    void featureUpdated( GeoDataFeature *feature );

private Q_SLOTS:
    void moveUp();
    void moveDown();
    void addFlyTo();
    void deleteSelected();

 private:
    Q_PRIVATE_SLOT( d, void openFile() )
    Q_PRIVATE_SLOT( d, void createTour() )
    Q_PRIVATE_SLOT( d, void saveTour() )
    Q_PRIVATE_SLOT( d, void saveTourAs() )
    Q_PRIVATE_SLOT( d, void updateButtonsStates() )
    Q_PRIVATE_SLOT( d, void mapCenterOn( const QModelIndex &index ) )
    Q_DISABLE_COPY( TourWidget )

    TourWidgetPrivate * const d;
};


class TourItemDelegate : public QStyledItemDelegate
{
Q_OBJECT

public:
    TourItemDelegate( QListView* view, MarbleWidget* widget );
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    void setIsEditing( QModelIndex index, bool newValue );

Q_SIGNALS:
    void editingChanged( QModelIndex index );

public:

    enum Element {
        GeoDataElementIcon,
        Label,
        EditButton,
        ActionButton
    };

protected:
    bool editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index );

private:
    static QRect position( Element element, const QStyleOptionViewItem &option );
    QList<QPersistentModelIndex> m_editingIndices;
    QListView* m_listView;
    MarbleWidget *m_widget;

};

}

#endif
