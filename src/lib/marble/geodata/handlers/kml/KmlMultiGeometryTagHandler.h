/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_KML_KMLMULTIGEOMETRYTAGHANDLER_H
#define MARBLE_KML_KMLMULTIGEOMETRYTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble 
{
namespace kml
{

class KmlMultiGeometryTagHandler : public GeoTagHandler 
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
