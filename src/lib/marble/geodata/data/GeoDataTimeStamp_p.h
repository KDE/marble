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

#include <QDateTime>

#include "GeoDataTypes.h"
#include <GeoDataTimeStamp.h>

namespace Marble
{

class GeoDataTimeStampPrivate
{
  public:
    QDateTime m_when;

    GeoDataTimeStamp::TimeResolution m_resolution;

    GeoDataTimeStampPrivate();
};

GeoDataTimeStampPrivate::GeoDataTimeStampPrivate() :
  m_resolution( GeoDataTimeStamp::SecondResolution )
{
  // nothing to do
}

} // namespace Marble

#endif //GEODATATIMESTAMPPRIVATE_H

