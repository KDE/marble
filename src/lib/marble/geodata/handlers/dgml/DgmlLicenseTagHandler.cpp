// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "DgmlLicenseTagHandler.h"
#include "GeoSceneLicense.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(License)

GeoNode *DgmlLicenseTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_License)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Head)) {
        QString const attribution = parser.attribute(dgmlAttr_attribution).trimmed().toLower();
        if (attribution == QLatin1StringView("never")) {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::Never);
        } else if (attribution == QLatin1StringView("opt-in") || attribution == QLatin1StringView("optin")) {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::OptIn);
        } else if (attribution.isEmpty() || attribution == QLatin1StringView("opt-out") || attribution == QLatin1StringView("optout")) {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::OptOut);
        } else if (attribution == QLatin1StringView("always")) {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::Always);
        } else {
            mDebug() << "Unknown license attribution value " << attribution << ", falling back to 'opt-out'.";
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution(GeoSceneLicense::OptOut);
        }
        QString const shortLicense = parser.attribute(dgmlAttr_short).trimmed();
        parentItem.nodeAs<GeoSceneHead>()->license()->setShortLicense(shortLicense);
        QString const fullLicense = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoSceneHead>()->license()->setLicense(fullLicense);
    }

    return nullptr;
}

}
}
