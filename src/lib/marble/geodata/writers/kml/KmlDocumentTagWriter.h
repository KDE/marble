// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_KMLDOCUMENTTAGWRITER_H
#define MARBLE_KMLDOCUMENTTAGWRITER_H

#include "GeoTagWriter.h"
#include "KmlFeatureTagWriter.h"

namespace Marble
{

class KmlDocumentTagWriter : public KmlFeatureTagWriter
{
public:
    KmlDocumentTagWriter();

protected:
    bool writeMid(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif
