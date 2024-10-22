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

static GeoTagWriterRegistrar s_writerListStyle(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataListStyleType),
                                                                           QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                               new KmlListStyleTagWriter());

bool KmlListStyleTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto listStyle = static_cast<const GeoDataListStyle *>(node);
    bool const isEmpty =
        listStyle->listItemType() == GeoDataListStyle::Check && listStyle->backgroundColor() == QColor(Qt::white) && listStyle->itemIconList().isEmpty();
    if (isEmpty) {
        return true;
    }
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_ListStyle));

    QString const itemType = itemTypeToString(listStyle->listItemType());
    writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_listItemType), itemType, QStringLiteral("check"));
    QString const color = KmlColorStyleTagWriter::formatColor(listStyle->backgroundColor());
    writer.writeOptionalElement(QLatin1String(kml::kmlTag_bgColor), color, QStringLiteral("ffffffff"));

    for (GeoDataItemIcon *icon : listStyle->itemIconList()) {
        writer.writeStartElement(QString::fromLatin1(kml::kmlTag_ItemIcon));
        QString const state = iconStateToString(icon->state());
        writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_state), state, QStringLiteral("open"));
        writer.writeOptionalElement(QString::fromLatin1(kml::kmlTag_href), icon->iconPath());
        writer.writeEndElement();
    }

    writer.writeEndElement();
    return true;
}

QString KmlListStyleTagWriter::itemTypeToString(GeoDataListStyle::ListItemType itemType)
{
    switch (itemType) {
    case GeoDataListStyle::CheckOffOnly:
        return QStringLiteral("checkOffOnly");
    case GeoDataListStyle::CheckHideChildren:
        return QStringLiteral("checkHideChildren");
    case GeoDataListStyle::RadioFolder:
        return QStringLiteral("radioFolder");
    default:
        return QStringLiteral("check");
    }
}

QString KmlListStyleTagWriter::iconStateToString(GeoDataItemIcon::ItemIconStates state)
{
    QStringList stateList;
    if (state & GeoDataItemIcon::Open) {
        stateList << QStringLiteral("open");
    }
    if (state & GeoDataItemIcon::Closed) {
        stateList << QStringLiteral("closed");
    }
    if (state & GeoDataItemIcon::Error) {
        stateList << QStringLiteral("error");
    }
    if (state & GeoDataItemIcon::Fetching0) {
        stateList << QStringLiteral("fetching0");
    }
    if (state & GeoDataItemIcon::Fetching1) {
        stateList << QStringLiteral("fetching1");
    }
    if (state & GeoDataItemIcon::Fetching2) {
        stateList << QStringLiteral("fetching2");
    }

    return stateList.join(QLatin1Char(' '));
}

}
