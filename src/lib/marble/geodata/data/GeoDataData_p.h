//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATADATAPRIVATE_H
#define GEODATADATAPRIVATE_H

#include <QString>
#include <QVariant>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDataPrivate
{
  public:
    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataDataType;
    }

    QVariant m_value;
    QString  m_name;
    QString  m_displayName;
};

} // namespace Marble

#endif //GEODATADATAPRIVATE_H
