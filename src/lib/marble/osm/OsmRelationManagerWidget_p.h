//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMRELATIONMANAGERWIDGETPRIVATE_H
#define MARBLE_OSMRELATIONMANAGERWIDGETPRIVATE_H

#include "ui_OsmRelationManagerWidget.h"

namespace Marble
{

class OsmRelationManagerWidget;
class GeoDataPlacemark;
class OsmPlacemarkData;

class OsmRelationManagerWidgetPrivate : public Ui::OsmRelationManagerWidgetPrivate
{
public:
    OsmRelationManagerWidgetPrivate();
    ~OsmRelationManagerWidgetPrivate();
    void populateRelationsList();
    void populateDropMenu();

private:
    friend class OsmRelationManagerWidget;
    GeoDataPlacemark *m_placemark;
    const QHash< qint64, OsmPlacemarkData > *m_allRelations;
    QMenu *m_relationDropMenu;
};

}

#endif
