// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef DGMLHEADTAGWRITER_H
#define DGMLHEADTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlHeadTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif // DGMLHEADTAGWRITER_H
