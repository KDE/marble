#include "KmlRangeTagHandler.h"

#include "KmlElementDictionary.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLookAt.h"
#include "GeoParser.h"
#include "MarbleGlobal.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(range)
GeoNode *KmlrangeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QString::fromLatin1(kmlTag_range)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataLookAt>()) {
        QString rangeTemp = parser.readElementText().trimmed();
        qreal range = rangeTemp.toDouble();

        parentItem.nodeAs<GeoDataLookAt>()->setRange(range);
    }

    return nullptr;
}
}
}
