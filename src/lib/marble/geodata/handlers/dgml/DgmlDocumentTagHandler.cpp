/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlDocumentTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "GeoSceneDocument.h"
#include "GeoSceneParser.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Document)

GeoNode *DgmlDocumentTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Document)));
    return geoSceneDoc(parser);
}

}
}
