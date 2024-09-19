// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef MARBLE_KMLMODELTAGWRITER_H
#define MARBLE_KMLMODELTAGWRITER_H

#include "GeoTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlModelTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
