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
    QString content = parser.readElementText().trimmed();
    GeoDataLink::RefreshMode mode;

    if ( parentItem.is<GeoDataLink>()) {
        if( content == QString( "onChange" ) ) {
            mode = GeoDataLink::OnChange;
        } else if( content == QString( "onExpire" ) ) {
            mode = GeoDataLink::OnExpire;
        } else if( content == QString( "onInterval" ) ) {
            mode = GeoDataLink::OnInterval;
        } else {
            mode = GeoDataLink::OnChange;
            mDebug() << "Value " << content << "set in kml file is invalid."
            << "Value of <refreshMode> has been reset to onChange" ;
        }
        parentItem.nodeAs<GeoDataLink>()->setRefreshMode( mode );
    }

    return 0;
}

}
}

