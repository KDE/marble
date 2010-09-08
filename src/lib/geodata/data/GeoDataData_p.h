//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATADATAPRIVATE_H
#define GEODATADATAPRIVATE_H

#include <QtCore/QString>
#include <QtCore/QVariant>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDataPrivate
{
  public:
    QString nodeType() const
    {
        return GeoDataTypes::GeoDataDataType;
    }

    QVariant m_value;
    QString  m_name;
    QString  m_displayName;
};

} // namespace Marble

#endif //GEODATADATAPRIVATE_H
