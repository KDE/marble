// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLMULTIGEOMETRYTAGWRITER_H
#define MARBLE_KMLMULTIGEOMETRYTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlMultiGeometryTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
