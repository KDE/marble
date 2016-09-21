//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMTAGEDITORWIDGET_H
#define MARBLE_OSMTAGEDITORWIDGET_H

#include <QWidget>
#include "marble_export.h"

#include "MarbleGlobal.h"

class QTreeWidgetItem;

namespace Marble
{

class OsmTagEditorWidgetPrivate;
class GeoDataPlacemark;
class GeoDataFeature;
class OsmPlacemarkData;

class MARBLE_EXPORT OsmTagEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OsmTagEditorWidget( GeoDataPlacemark *placemark, QWidget *parent = 0 );
    ~OsmTagEditorWidget();

    /**
     * @brief returns the current tag set for the placemark
     */
    OsmPlacemarkData placemarkData() const;

public Q_SLOTS:
    void update();
    void addSelectedTag();
    void removeSelectedTag();
    void handleDoubleClick( QTreeWidgetItem *item, int column );
    void handleItemChanged( QTreeWidgetItem *item, int column );

Q_SIGNALS:
    void placemarkChanged( GeoDataFeature *);

private:
    friend class OsmTagEditorWidgetPrivate;
    OsmTagEditorWidgetPrivate* const d;
};

}

#endif
 
