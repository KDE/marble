/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_KML_KMLSIMPLEFIELDTAGHANDLER_H
#define MARBLE_KML_KMLSIMPLEFIELDTAGHANDLER_H

#include "GeoTagHandler.h"

#include "GeoDataSimpleField.h"

class QString;

namespace Marble
{
namespace kml
{

class KmlSimpleFieldTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;

private:
    static GeoDataSimpleField::SimpleFieldType resolveType(const QString &type);
};

}
}

#endif
