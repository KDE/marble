#ifndef MARBLE_KML_KMLREFRESHMODETAGHANDLER_H
#define MARBLE_KML_KMLREFRESHMODETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlrefreshModeTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
