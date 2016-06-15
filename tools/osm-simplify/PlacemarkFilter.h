#ifndef PLACEMARKHANDLER_H
#define PLACEMARKHANDLER_H

#include "BaseFilter.h"

class PlacemarkFilter : public BaseFilter
{
public:
    PlacemarkFilter(GeoDataDocument* document, const char *type);
};

#endif // PLACEMARKHANDLER_H
