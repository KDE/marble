// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_KMLFEATURETAGWRITER_H
#define MARBLE_KMLFEATURETAGWRITER_H

#include "GeoTagWriter.h"

#include <QString>

namespace Marble
{

// No registration for this writer, ColorStyle is an abstract kml element
class KmlFeatureTagWriter : public GeoTagWriter
{
public:
    explicit KmlFeatureTagWriter(const QString &elementName);

    bool write(const GeoNode *node, GeoWriter &writer) const override;

protected:
    virtual bool writeMid(const GeoNode *node, GeoWriter &writer) const = 0;

private:
    QString const m_elementName;
};

}

#endif
