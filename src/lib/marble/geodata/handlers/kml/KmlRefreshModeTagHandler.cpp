#include "KmlRefreshModeTagHandler.h"
#include "GeoDataLink.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(refreshMode)

GeoNode *KmlrefreshModeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_refreshMode)));

    GeoStackItem parentItem = parser.parentElement();
    QString content = parser.readElementText().trimmed();
    GeoDataLink::RefreshMode mode;

    if (parentItem.is<GeoDataLink>()) {
        if (content == QLatin1StringView("onChange")) {
            mode = GeoDataLink::OnChange;
        } else if (content == QLatin1StringView("onExpire")) {
            mode = GeoDataLink::OnExpire;
        } else if (content == QLatin1StringView("onInterval")) {
            mode = GeoDataLink::OnInterval;
        } else {
            mode = GeoDataLink::OnChange;
            mDebug() << "Value " << content << "set in kml file is invalid."
                     << "Value of <refreshMode> has been reset to onChange";
        }
        parentItem.nodeAs<GeoDataLink>()->setRefreshMode(mode);
    }

    return nullptr;
}

}
}
