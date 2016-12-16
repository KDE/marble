#ifndef KmlRangeTagHandler_h
#define KmlRangeTagHandler_h

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlrangeTagHandler:public GeoTagHandler
{
public:
    GeoNode * parse (GeoParser &) const override;
};

}
}

#endif
