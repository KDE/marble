//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATATIMESTAMPPRIVATE_H
#define GEODATATIMESTAMPPRIVATE_H

#include <QtCore/QDateTime>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataTimeStampPrivate
{
  public:
    QString nodeType() const
    {
        return GeoDataTypes::GeoDataTimeStampType;
    }

    QDateTime m_when;
};

} // namespace Marble

#endif //GEODATATIMESTAMPPRIVATE_H

