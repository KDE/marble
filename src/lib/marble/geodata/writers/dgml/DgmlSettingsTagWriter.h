// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef DGMLSETTINGSTAGWRITER_H
#define DGMLSETTINGSTAGWRITER_H

#include "GeoTagWriter.h"

namespace Marble
{

class DgmlSettingsTagWriter : public GeoTagWriter
{
public:
    bool write(const GeoNode *node, GeoWriter &writer) const override;
};

}

#endif // DGMLSETTINGSTAGWRITER_H
