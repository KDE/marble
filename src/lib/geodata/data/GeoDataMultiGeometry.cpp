//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#include "GeoDataMultiGeometry.h"

GeoDataMultiGeometry::GeoDataMultiGeometry()
{
}

GeoDataMultiGeometry::GeoDataMultiGeometry( const GeoDataMultiGeometry& other )
        : QVector<GeoDataGeometry>( other ),
          GeoDataGeometry( other )
{
}

GeoDataMultiGeometry::~GeoDataMultiGeometry()
{
}
