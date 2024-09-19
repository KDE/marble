/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_KML_KMLDOCUMENTTAGHANDLER_H
#define MARBLE_KML_KMLDOCUMENTTAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace kml
{

class KmlDocumentTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif
