// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef DGMLMAPTAGWRITER_H
#define DGMLMAPTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlMapTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif // DGMLMAPTAGWRITER_H
