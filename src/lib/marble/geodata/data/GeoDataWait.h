//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#ifndef GEODATAWAIT_H
#define GEODATAWAIT_H

#include "GeoDataTourPrimitive.h"

namespace Marble
{

class GEODATA_EXPORT GeoDataWait : public GeoDataTourPrimitive
{
public:

    GeoDataWait();
    ~GeoDataWait();

    const char *nodeType() const;

    QString id() const;
    void setId(const QString &id);

    double duration() const;
    void setDuration(double duration);

private:
    QString m_id;
    double m_duration;
};

} // namespace Marble

#endif // GEODATATOURCONTROL_H
