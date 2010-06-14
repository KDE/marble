//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATAEXTENDEDDATAPRIVATE_H
#define GEODATAEXTENDEDDATAPRIVATE_H

#include <QtCore/QString>
#include <QtCore/QHash>

#include "GeoDataData.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataExtendedDataPrivate
{
  public:

    QString nodeType() const
    {
        return GeoDataTypes::GeoDataExtendedDataType;
    }

    QHash< QString, GeoDataData > hash;

};

} // namespace Marble

#endif //GEODATAEXTENDEDDATAPRIVATE_H
