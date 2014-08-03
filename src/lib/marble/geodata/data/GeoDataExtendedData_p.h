//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATAEXTENDEDDATAPRIVATE_H
#define GEODATAEXTENDEDDATAPRIVATE_H

#include <QString>
#include <QHash>

#include "GeoDataData.h"
#include "GeoDataSimpleArrayData.h"
#include "GeoDataSchemaData.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataExtendedDataPrivate
{
  public:

    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataExtendedDataType;
    }

    QHash< QString, GeoDataData > hash;
    QHash< QString, GeoDataSimpleArrayData* > arrayHash;
    QHash< QString, GeoDataSchemaData> schemaDataHash;

};

} // namespace Marble

#endif //GEODATAEXTENDEDDATAPRIVATE_H
