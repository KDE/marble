// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyatf@gmail.com>
//

#ifndef MARBLE_KMLPOLYGONTAGWRITER_H
#define MARBLE_KMLPOLYGONTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlPolygonTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
