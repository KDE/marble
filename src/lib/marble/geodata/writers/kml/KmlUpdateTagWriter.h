// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Marek Hakala <hakala.marek@gmail.com>
//

#ifndef MARBLE_KMLUPDATETAGWRITER_H
#define MARBLE_KMLUPDATETAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class KmlUpdateTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}
#endif
