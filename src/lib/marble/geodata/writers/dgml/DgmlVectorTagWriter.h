// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_DGMLVECTORTAGWRITER_H
#define MARBLE_DGMLVECTORTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlVectorTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
