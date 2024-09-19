// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//

#ifndef MARBLE_KMLFOLDERTAGWRITER_H
#define MARBLE_KMLFOLDERTAGWRITER_H

#include "GeoTagWriter.h"
#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlFolderTagWriter : public KmlFeatureTagWriter
{
public:
    KmlFolderTagWriter();

protected:
    bool writeMid(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
