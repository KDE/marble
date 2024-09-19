// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMTAGWRITER_H
#define MARBLE_OSMTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class OsmTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
