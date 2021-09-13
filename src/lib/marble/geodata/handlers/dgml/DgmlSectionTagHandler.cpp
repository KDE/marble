/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlSectionTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSection.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Section)

GeoNode* DgmlSectionTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Section)));

    QString name      = parser.attribute(dgmlAttr_name);
    QString checkable = parser.attribute(dgmlAttr_checkable).toLower().trimmed();
    QString connectTo = parser.attribute(dgmlAttr_connect).trimmed();
    QString radio = parser.attribute(dgmlAttr_radio);
    int     spacing   = parser.attribute(dgmlAttr_spacing).toInt();

    GeoSceneSection* section = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Legend)) {
        section = new GeoSceneSection( name );
        section->setCheckable( checkable == dgmlValue_true || checkable == dgmlValue_on );
        section->setConnectTo( connectTo );
        section->setSpacing( spacing );
        section->setRadio( radio );
        parentItem.nodeAs<GeoSceneLegend>()->addSection( section );
    }

    return section;
}

}
}
