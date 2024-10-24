// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// Self
#include "OsmRelationManagerWidget_p.h"
#include "OsmRelationManagerWidget.h"

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "MarbleDebug.h"
#include "OsmPlacemarkData.h"

// Qt
#include <QMenu>
#include <QTreeWidget>

namespace Marble
{

OsmRelationManagerWidgetPrivate::OsmRelationManagerWidgetPrivate()
{
    // nothing to do
}

OsmRelationManagerWidgetPrivate::~OsmRelationManagerWidgetPrivate()
{
    // nothing to do
}

void OsmRelationManagerWidgetPrivate::populateRelationsList()
{
    m_currentRelations->clear();

    // This shouldn't happen
    if (!m_allRelations) {
        return;
    }

    if (m_placemark->hasOsmData()) {
        const OsmPlacemarkData &osmData = m_placemark->osmData();
        auto it = osmData.relationReferencesBegin();
        const auto end = osmData.relationReferencesEnd();

        for (; it != end; ++it) {
            if (!m_allRelations->contains(it.key().id)) {
                mDebug() << QStringLiteral("Relation %1 is not loaded in the Annotate Plugin").arg(it.key().id);
                continue;
            }

            const OsmPlacemarkData &relationData = m_allRelations->value(it.key().id);

            auto newItem = new QTreeWidgetItem();
            QString name = relationData.tagValue(QStringLiteral("name"));
            QString type = relationData.tagValue(QStringLiteral("type"));
            QString role = it.value();
            newItem->setText(Column::Name, name);
            newItem->setText(Column::Type, type);
            newItem->setText(Column::Role, role);
            newItem->setData(Column::Name, Qt::UserRole, relationData.id());
            m_currentRelations->addTopLevelItem(newItem);
        }
    }
}

void OsmRelationManagerWidgetPrivate::populateDropMenu()
{
    m_relationDropMenu->clear();

    m_addRelation->setIcon(QIcon(QStringLiteral(":marble/list-add.png")));

    // The new relation adder
    m_relationDropMenu->addAction(QObject::tr("New Relation"));
    m_relationDropMenu->addSeparator();

    // This shouldn't happen
    Q_ASSERT(m_allRelations);

    // Suggesting existing relations
    for (const OsmPlacemarkData &relationData : m_allRelations->values()) {
        const QString relationText =
            relationData.tagValue(QStringLiteral("name")) + QLatin1StringView(" (") + relationData.tagValue(QStringLiteral("type")) + QLatin1Char(')');

        // Don't suggest relations the placemark is already part of
        if (m_placemark->hasOsmData() && m_placemark->osmData().containsRelation(relationData.id())) {
            continue;
        }
        auto newAction = new QAction(m_relationDropMenu);
        newAction->setText(relationText);
        newAction->setData(relationData.id());
        m_relationDropMenu->addAction(newAction);
    }
}

}
