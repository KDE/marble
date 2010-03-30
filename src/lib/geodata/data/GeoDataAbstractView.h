#ifndef GEODATAABSTRACTVIEW_H
#define GEODATAABSTRACTVIEW_H

#include "GeoDataObject.h"

#include "geodata_export.h"

namespace Marble
{

class GEODATA_EXPORT GeoDataAbstractView : public GeoDataObject
{
 public:
    GeoDataAbstractView();
    ~GeoDataAbstractView();
};

} // namespace Marble

#endif

