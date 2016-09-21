//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMRELATIONMANAGERWIDGET_H
#define MARBLE_OSMRELATIONMANAGERWIDGET_H

#include <QWidget>

#include "MarbleGlobal.h"
#include "marble_export.h"

class QTreeWidgetItem;

namespace Marble
{

class OsmRelationManagerWidgetPrivate;
class GeoDataPlacemark;
class OsmPlacemarkData;

enum Column {
    Name = 0,
    Type = 1,
    Role = 2
};

/**
 * @brief The OsmRelationManagerWidget class is used to view and edit relations for any placemark
 * within the Annotate Plugin.
 * It can add the placemark to any relation the Annotate Plugin has loaded, or to newly created ones
 */
class MARBLE_EXPORT OsmRelationManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OsmRelationManagerWidget( GeoDataPlacemark *placemark,
                                       const QHash<qint64, OsmPlacemarkData> *relations,
                                       QWidget *parent = 0 );
    ~OsmRelationManagerWidget();

public Q_SLOTS:
    /**
     * @brief addRelation adds the placemark to the relation specified in the action->text();
     * If the text is "New Relation", a new relation is added
     */
    void addRelation( QAction* action );
    void handleItemChange( QTreeWidgetItem *item, int column );
    void handleDoubleClick( QTreeWidgetItem * item, int column );
    void handleRelationContextMenuRequest( const QPoint& point );
    /**
     * @brief update updates the relations list and the suggested relations drop menu
     */
    void update();

Q_SIGNALS:
    void relationCreated( const OsmPlacemarkData &relationData );

private:
    friend class OsmRelationManagerWidgetPrivate;
    OsmRelationManagerWidgetPrivate* const d;
};

}

#endif
 
