// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "DgmlTagWriter.h"

#include "DgmlElementDictionary.h"
#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerDgml(GeoTagWriter::QualifiedName({}, QString::fromLatin1(dgml::dgmlTag_nameSpace20)), new DgmlTagWriter());

bool DgmlTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    Q_UNUSED(node);
    writer.writeStartElement("dgml");
    writer.writeAttribute("xmlns", QString::fromUtf8(dgml::dgmlTag_nameSpace20));

    // Do not write an end element for document handlers
    return true;
}

}
