// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Shou Ya <shouyalovelyx@gmail.com>
//

#ifndef MARBLE_KMLTIMESPANWRITER_H
#define MARBLE_KMLTIMESPANWRITER_H

#include "GeoTagWriter.h"

#include "MarbleGlobal.h"

namespace Marble
{

class KmlTimeSpanWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
