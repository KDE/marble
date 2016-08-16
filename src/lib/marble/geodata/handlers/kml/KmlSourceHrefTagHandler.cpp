//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlSourceHrefTagHandler.h"

#include "KmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataAlias.h"


namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( sourceHref )

GeoNode* KmlsourceHrefTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_sourceHref)));

    GeoStackItem parentItem = parser.parentElement();

    QString content = parser.readElementText().trimmed();

    if ( parentItem.is<GeoDataAlias>() ){
        parentItem.nodeAs<GeoDataAlias>()->setSourceHref( content );
    }

    return 0;
}

}
}
