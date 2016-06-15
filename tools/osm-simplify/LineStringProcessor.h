#ifndef LINESTRINGHANDLER_H
#define LINESTRINGHANDLER_H

#include <QList>

#include "PlacemarkFilter.h"

class LineStringProcessor : public PlacemarkFilter
{
public:
    LineStringProcessor(GeoDataDocument* document);

    virtual void process();

};

#endif // LINESTRINGHANDLER_H
