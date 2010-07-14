//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef GEODATATIMESPANPRIVATE_H
#define GEODATATIMESPANPRIVATE_H

#include <QtCore/QDateTime>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataTimeSpanPrivate
{
  public:
    QString nodeType() const
    {
        return GeoDataTypes::GeoDataTimeSpanType;
    }

    QDateTime m_begin;
    QDateTime m_end;
};

} // namespace Marble

#endif //GEODATATIMESPANPRIVATE_H

