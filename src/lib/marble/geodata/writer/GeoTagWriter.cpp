// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
//

#include "GeoTagWriter.h"

#include "GeoWriter.h"

#include <QDebug>

namespace Marble
{

GeoTagWriter::GeoTagWriter() = default;

GeoTagWriter::~GeoTagWriter() = default;

bool GeoTagWriter::writeElement(const GeoNode *object, GeoWriter &writer)
{
    return writer.writeElement(object);
}

void GeoTagWriter::registerWriter(const QualifiedName &name, const GeoTagWriter *writer)
{
    TagHash *tagHash = tagWriterHash();

    if (tagHash->contains(name)) {
        qWarning()
            << "Warning: The tag" << name
            << "is already registered. Often this indicates that multiple versions of the marblewidget library are loaded at the same time. This will likely "
               "lead to problems. Please check your installation, especially internal Marble plugins and external applications that install Marble plugins.";
    }
    Q_ASSERT(!tagHash->contains(name));
    tagHash->insert(name, writer);
    Q_ASSERT(tagHash->contains(name));
}

void GeoTagWriter::unregisterWriter(const GeoTagWriter::QualifiedName &qName)
{
    auto hash = tagWriterHash();
    Q_ASSERT(hash->contains(qName));
    delete hash->value(qName);
    hash->remove(qName);
    Q_ASSERT(!hash->contains(qName));
}

GeoTagWriter::TagHash *GeoTagWriter::tagWriterHash()
{
    static TagHash s_tagWriterHash;
    return &s_tagWriterHash;
}

const GeoTagWriter *GeoTagWriter::recognizes(const QualifiedName &qname)
{
    TagHash *hash = tagWriterHash();

    if (!hash->contains(qname)) {
        return nullptr;
    }

    return hash->value(qname);
}

}
