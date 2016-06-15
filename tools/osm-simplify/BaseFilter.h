#ifndef OBJECTHANDLER_H
#define OBJECTHANDLER_H

#include <QList>
#include <QDebug>

#include "GeoDataObject.h"
#include "GeoDataDocument.h"
#include "GeoDataTypes.h"

using namespace Marble;

class BaseFilter
{
public:
    BaseFilter(GeoDataDocument* document, const char *type);
    virtual ~BaseFilter();

    virtual void process() = 0;

protected:
    GeoDataDocument* m_document;
    QList<GeoDataObject*> m_objects;
};

#endif // OBJECTHANDLER_H
