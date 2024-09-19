/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_KML_KMLsCALETAGHANDLER_H
#define MARBLE_KML_KMLsCALETAGHANDLER_H

// be aware that there is another Tag called Scale which uses the name KmlscaleTagHandler.h
// as it is impossible to use the name KmlscaleTagHandler then, use an underscore
// to mark the lower case variant
// FIXME: is this really the case nowadays?
// There does not seem to be a KmlscaleTagHandler.h, nor seems kmlTag_Scale to be used anymore.
// As a reminder let's keep the small 's' for now.

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlscaleTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
