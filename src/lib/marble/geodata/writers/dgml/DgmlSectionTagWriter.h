// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef DGMLSECTIONTAGWRITER_H
#define DGMLSECTIONTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlSectionTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif // DGMLSECTIONTAGWRITER_H
