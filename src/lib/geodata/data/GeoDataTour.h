//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef GEODATATOUR_H
#define GEODATATOUR_H

#include "GeoDataFeature.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataTourPrivate;

class GEODATA_EXPORT GeoDataTour : public GeoDataFeature
{
public:
    GeoDataTour();
    virtual ~GeoDataTour();

    QString id() const;
    void setId(QString value);

private:
    GeoDataTourPrivate * const d;

};

} // namespace Marble

#endif // GEODATATOUR_H
