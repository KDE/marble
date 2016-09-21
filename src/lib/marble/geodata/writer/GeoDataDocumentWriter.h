//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_GEODATADOCUMENTWRITER_H
#define MARBLE_GEODATADOCUMENTWRITER_H

#include "marble_export.h"

#include <QIODevice>
#include <QSet>

namespace Marble
{

class GeoWriterBackend;
class GeoDataDocument;

/**
 * Write GeoDataDocument content to I/O devices (e.g. a file)
 */
class MARBLE_EXPORT GeoDataDocumentWriter
{
public:
    /**
     * Write the content of the given GeoDataDocument to the given I/O device
     * @param device An I/O device open for writing
     * @param document A GeoDataDocument with content to write
     * @param documentIdentifier XML document identifier or file extension that determines
     * @return True if writing is successful, false otherwise
     */
    static bool write(QIODevice* device, const GeoDataDocument &document, const QString &documentIdentifier);

    /**
     * Convenience method that uses a QFile as QIODevice and determines the document type from the filename extension
     * @param filename Target file's name
     * @param document Document to write
     * @param documentIdentifier XML document identifier or filename extension that determines the content type.
     * Use an empty string (default) to determine it automatically
     * @return True if writing is successful, false otherwise
     */
    static bool write(const QString &filename, const GeoDataDocument &document, const QString &documentIdentifier = QString());

    /**
     * Registers a plugin as a backend for writing a certain file extension. Usually called by the MARBLE_ADD_WRITER macro.
     * @param writer Backend to register
     * @param fileExtension File extension to associate the backend with
     */
    static void registerWriter(GeoWriterBackend* writer, const QString &fileExtension);

private:
    static QString determineDocumentIdentifier(const QString &filename);
    static QSet<QPair<QString, GeoWriterBackend*> > s_backends;
};

}

#endif
