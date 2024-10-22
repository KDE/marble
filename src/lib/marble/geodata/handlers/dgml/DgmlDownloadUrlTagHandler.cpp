/*
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "DgmlDownloadUrlTagHandler.h"

// Qt
#include <QUrl>
#include <QUrlQuery>

// Marble
#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(DownloadUrl)

GeoNode *DgmlDownloadUrlTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_DownloadUrl)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (!parentItem.represents(dgmlTag_Texture) && !parentItem.represents(dgmlTag_Vectortile))
        return nullptr;

    QUrl url;

    // Attribute protocol, optional?
    QString protocolStr = parser.attribute(dgmlAttr_protocol).trimmed();
    if (!protocolStr.isEmpty())
        url.setScheme(protocolStr);

    // Attribute user, optional
    const QString userStr = parser.attribute(dgmlAttr_user).trimmed();
    if (!userStr.isEmpty())
        url.setUserName(userStr);

    // Attribute password, optional
    const QString passwordStr = parser.attribute(dgmlAttr_password).trimmed();
    if (!passwordStr.isEmpty())
        url.setPassword(passwordStr);

    // Attribute host, mandatory
    const QString hostStr = parser.attribute(dgmlAttr_host).trimmed();
    if (!hostStr.isEmpty())
        url.setHost(hostStr);

    // Attribute port, optional
    const QString portStr = parser.attribute(dgmlAttr_port).trimmed();
    if (!portStr.isEmpty())
        url.setPort(portStr.toInt());

    // Attribute path, mandatory
    const QString pathStr = parser.attribute(dgmlAttr_path).trimmed();
    if (!pathStr.isEmpty())
        url.setPath(pathStr);

    // Attribute query, optional
    const QString queryStr = parser.attribute(dgmlAttr_query).trimmed();
    if (!queryStr.isEmpty()) {
        QUrlQuery query;
        query.setQuery(queryStr);
        url.setQuery(query);
    }

    parentItem.nodeAs<GeoSceneTileDataset>()->addDownloadUrl(url);
    return nullptr;
}

}
}
