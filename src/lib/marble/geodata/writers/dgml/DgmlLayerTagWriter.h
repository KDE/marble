// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef DGMLLAYERTAGWRITER_H
#define DGMLLAYERTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlLayerTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif // DGMLLAYERTAGWRITER_H
