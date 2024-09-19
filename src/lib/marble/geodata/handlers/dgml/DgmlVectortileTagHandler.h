/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_DGML_VECTORTILETAGHANDLER_H
#define MARBLE_DGML_VECTORTILETAGHANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace dgml
{

class DgmlVectortileTagHandler : public GeoTagHandler
{
public:
    GeoNode *parse(GeoParser &) const override;
};

}
}

#endif // MARBLE_DGML_VECTORTILETAGHANDLER_H
