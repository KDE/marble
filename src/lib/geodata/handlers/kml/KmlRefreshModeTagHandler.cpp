#include "KmlRefreshModeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataLink.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER( refreshMode )

GeoNode* KmlrefreshModeTagHandler::parse( GeoParser& parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( kmlTag_refreshMode ) );
    GeoStackItem parentItem = parser.parentElement();
    if ( parentItem.is<GeoDataLink>()) {
        QString content = parser.readElementText().trimmed();

        GeoDataLink::RefreshMode mode = GeoDataLink::OnChange;
        if( content == QString( "onExpire" ) ) {
            mode = GeoDataLink::OnExpire;
        } else if( content == QString( "onInterval" ) ) {
            mode = GeoDataLink::OnInterval;
        }

        parentItem.nodeAs<GeoDataLink>()->setRefreshMode( mode );
    }

    return 0;
}

}
}

