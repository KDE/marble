//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "GeoWriterBackend.h"

#include "GeoDataDocumentWriter.h"

namespace Marble {

GeoWriterBackend::~GeoWriterBackend()
{
    // does nothing
}

GeoWriterBackendRegistrar::GeoWriterBackendRegistrar(GeoWriterBackend *writer, const QString &fileExtension)
{
    GeoDataDocumentWriter::registerWriter(writer, fileExtension);
}

}
