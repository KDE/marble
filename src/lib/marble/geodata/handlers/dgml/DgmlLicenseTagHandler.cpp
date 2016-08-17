//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright (C) 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "DgmlLicenseTagHandler.h"
#include "GeoSceneLicense.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneHead.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(License)

GeoNode* DgmlLicenseTagHandler::parse( GeoParser& parser ) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_License)));

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.represents( dgmlTag_Head ) ) {
        QString const attribution = parser.attribute(dgmlAttr_attribution).trimmed().toLower();
        if (attribution == QLatin1String("never")) {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution( GeoSceneLicense::Never );
        } else if (attribution == QLatin1String("opt-in") || attribution == QLatin1String("optin")) {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution( GeoSceneLicense::OptIn );
        } else if (attribution.isEmpty() || attribution == QLatin1String("opt-out") || attribution == QLatin1String("optout")) {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution( GeoSceneLicense::OptOut );
        } else if (attribution == QLatin1String("always")) {
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution( GeoSceneLicense::Always );
        } else {
            mDebug() << "Unknown license attribution value " << attribution << ", falling back to 'opt-out'.";
            parentItem.nodeAs<GeoSceneHead>()->license()->setAttribution( GeoSceneLicense::OptOut );
        }
        QString const shortLicense = parser.attribute( dgmlAttr_short ).trimmed();
        parentItem.nodeAs<GeoSceneHead>()->license()->setShortLicense( shortLicense );
        QString const fullLicense = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoSceneHead>()->license()->setLicense( fullLicense );
    }

    return 0;
}

}
}
