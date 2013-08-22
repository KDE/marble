//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef GEODATAPLAYLIST_H
#define GEODATAPLAYLIST_H

#include "GeoDataObject.h"
#include "GeoDataTourPrimitive.h"

#include <QList>

namespace Marble
{

class GEODATA_EXPORT GeoDataPlaylist : public GeoDataObject
{
public:
    GeoDataPlaylist();
    ~GeoDataPlaylist();

    const char *nodeType() const;

    GeoDataTourPrimitive* primitive(int id);
    const GeoDataTourPrimitive* primitive(int id) const;
    void addPrimitive(GeoDataTourPrimitive* primitive);

    int size() const;

private:
    QList<GeoDataTourPrimitive*> m_primitives;
};

} // namespace Marble

#endif // GEODATAPLAYLIST_H
