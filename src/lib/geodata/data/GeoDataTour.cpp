//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoDataTour.h"

namespace Marble
{

class GeoDataTourPrivate
{
public:
    GeoDataTourPrivate() : m_id("") {}
    QString m_id;
};

GeoDataTour::GeoDataTour() :
    GeoDataFeature(),
    d(new GeoDataTourPrivate)
{
}

GeoDataTour::~GeoDataTour()
{
    delete d;
}

QString GeoDataTour::id() const
{
    return d->m_id;
}

void GeoDataTour::setId(QString value)
{
    d->m_id = value;
}

} // namespace Marble
