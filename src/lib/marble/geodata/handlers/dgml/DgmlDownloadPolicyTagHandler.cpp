/*
    SPDX-FileCopyrightText: 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "DgmlDownloadPolicyTagHandler.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"

#include "MarbleGlobal.h"

namespace Marble
{
namespace dgml
{
static GeoTagHandlerRegistrar handler(GeoParser::QualifiedName(QString::fromLatin1(dgmlTag_DownloadPolicy), QString::fromLatin1(dgmlTag_nameSpace20)),
                                      new DgmlDownloadPolicyTagHandler);

// Error handling:
// Here it is not possible to return an error code or throw an exception
// so in case of an error we just ignore the element.

GeoNode *DgmlDownloadPolicyTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_DownloadPolicy)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (!parentItem.represents(dgmlTag_Texture) && !parentItem.represents(dgmlTag_Vectortile)) {
        qCritical("Parse error: parent element is not 'texture' or 'vectortile'");
        return nullptr;
    }

    // Attribute usage
    DownloadUsage usage;
    const QString usageStr = parser.attribute(dgmlAttr_usage).trimmed();
    if (usageStr == QLatin1StringView("Browse"))
        usage = DownloadBrowse;
    else if (usageStr == QLatin1StringView("Bulk"))
        usage = DownloadBulk;
    else {
        qCritical("Parse error: invalid attribute downloadPolicy/@usage");
        return nullptr;
    }

    // Attribute maximumConnections
    const QString maximumConnectionsStr = parser.attribute(dgmlAttr_maximumConnections).trimmed();
    bool ok;
    const int maximumConnections = maximumConnectionsStr.toInt(&ok);
    if (!ok) {
        qCritical("Parse error: invalid attribute downloadPolicy/@maximumConnections");
        return nullptr;
    }

    parentItem.nodeAs<GeoSceneTileDataset>()->addDownloadPolicy(usage, maximumConnections);
    return nullptr;
}

}
}
