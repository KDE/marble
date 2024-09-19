// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Adrian Draghici <draghici.adrian.b@gmail.com>
//

#ifndef MARBLE_KMLTOURTAGWRITER_H
#define MARBLE_KMLTOURTAGWRITER_H

#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlTourTagWriter : public KmlFeatureTagWriter
{
public:
    KmlTourTagWriter();

    bool writeMid(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
