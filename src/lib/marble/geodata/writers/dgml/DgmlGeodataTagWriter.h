// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_DGMLGEODATATAGWRITER_H
#define MARBLE_DGMLGEODATATAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlGeodataTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
