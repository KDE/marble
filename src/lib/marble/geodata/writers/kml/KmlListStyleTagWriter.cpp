// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mayank Madan <maddiemadan@gmail.com>
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlListStyleTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlColorStyleTagWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerListStyle(GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataListStyleType, kml::kmlTag_nameSpaceOgc22),
                                               new KmlListStyleTagWriter());

bool KmlListStyleTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const GeoDataListStyle *listStyle = static_cast<const GeoDataListStyle *>(node);
    bool const isEmpty =
        listStyle->listItemType() == GeoDataListStyle::Check && listStyle->backgroundColor() == QColor(Qt::white) && listStyle->itemIconList().isEmpty();
    if (isEmpty) {
        return true;
    }
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_ListStyle));

    QString const itemType = itemTypeToString(listStyle->listItemType());
    writer.writeOptionalElement(kml::kmlTag_listItemType, itemType, "check");
    QString const color = KmlColorStyleTagWriter::formatColor(listStyle->backgroundColor());
    writer.writeOptionalElement(kml::kmlTag_bgColor, color, "ffffffff");

    for (GeoDataItemIcon *icon : listStyle->itemIconList()) {
        writer.writeStartElement(QString::fromUtf8(kml::kmlTag_ItemIcon));
        QString const state = iconStateToString(icon->state());
        writer.writeOptionalElement(kml::kmlTag_state, state, "open");
        writer.writeOptionalElement(kml::kmlTag_href, icon->iconPath());
        writer.writeEndElement();
    }

    writer.writeEndElement();
    return true;
}

QString KmlListStyleTagWriter::itemTypeToString(GeoDataListStyle::ListItemType itemType)
{
    switch (itemType) {
    case GeoDataListStyle::CheckOffOnly:
        return "checkOffOnly";
    case GeoDataListStyle::CheckHideChildren:
        return "checkHideChildren";
    case GeoDataListStyle::RadioFolder:
        return "radioFolder";
    default:
        return "check";
    }
}

QString KmlListStyleTagWriter::iconStateToString(GeoDataItemIcon::ItemIconStates state)
{
    QStringList stateList;
    if (state & GeoDataItemIcon::Open) {
        stateList << "open";
    }
    if (state & GeoDataItemIcon::Closed) {
        stateList << "closed";
    }
    if (state & GeoDataItemIcon::Error) {
        stateList << "error";
    }
    if (state & GeoDataItemIcon::Fetching0) {
        stateList << "fetching0";
    }
    if (state & GeoDataItemIcon::Fetching1) {
        stateList << "fetching1";
    }
    if (state & GeoDataItemIcon::Fetching2) {
        stateList << "fetching2";
    }

    return stateList.join(QLatin1Char(' '));
}

}
