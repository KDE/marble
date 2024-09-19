// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef DGMLTEXTURETAGWRITER_H
#define DGMLTEXTURETAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlTextureTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif // DGMLTEXTURETAGWRITER_H
