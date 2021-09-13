// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
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
    QHash< QString, GeoDataData > hash;
    QHash< QString, GeoDataSimpleArrayData* > arrayHash;
    QHash< QString, GeoDataSchemaData> schemaDataHash;

};

} // namespace Marble

#endif //GEODATAEXTENDEDDATAPRIVATE_H
