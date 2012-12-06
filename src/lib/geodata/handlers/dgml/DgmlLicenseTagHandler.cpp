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

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(License)

GeoNode* DgmlLicenseTagHandler::parse( GeoParser& parser ) const
{
    // Check whether the tag is valid
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( dgmlTag_License ) );

    GeoStackItem parentItem = parser.parentElement();
    if( parentItem.represents( dgmlTag_Head ) ) {
        QString const shortLicense = parser.attribute( dgmlAttr_short ).trimmed();
        parentItem.nodeAs<GeoSceneHead>()->license()->setShortLicense( shortLicense );
        QString const fullLicense = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoSceneHead>()->license()->setLicense( fullLicense );
    }

    return 0;
}

}
}
