// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_KMLSCHEMADATATAGWRITER_H
#define MARBLE_KMLSCHEMADATATAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlSchemaDataTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
