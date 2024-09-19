// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_KMLPLACEMARKTAGWRITER_H
#define MARBLE_KMLPLACEMARKTAGWRITER_H

#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlPlacemarkTagWriter : public KmlFeatureTagWriter
{
public:
    KmlPlacemarkTagWriter();

protected:
    bool writeMid(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
