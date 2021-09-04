// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#include "GeoWriterBackend.h"

#include "GeoDataDocumentWriter.h"

namespace Marble {

GeoWriterBackend::~GeoWriterBackend()
{
    // does nothing
}

GeoWriterBackendRegistrar::GeoWriterBackendRegistrar(GeoWriterBackend *writer, const QString &fileExtension) :
    m_writer(writer),
    m_fileExtension(fileExtension)
{
    GeoDataDocumentWriter::registerWriter(writer, fileExtension);
}

GeoWriterBackendRegistrar::~GeoWriterBackendRegistrar()
{
    GeoDataDocumentWriter::unregisterWriter(m_writer, m_fileExtension);
}

}
