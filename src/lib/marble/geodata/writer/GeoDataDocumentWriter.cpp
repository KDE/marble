//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "GeoDataDocumentWriter.h"

#include "GeoWriterBackend.h"
#include "GeoWriter.h"
#include "GeoTagWriter.h"
#include "GeoDataDocument.h"
#include "KmlElementDictionary.h"

#include <QFileInfo>
#include <MarbleDebug.h>

namespace Marble {

QSet<QPair<QString, GeoWriterBackend*> > GeoDataDocumentWriter::s_backends;

bool GeoDataDocumentWriter::write(QIODevice *device, const GeoDataDocument &document, const QString &documentIdentifier)
{
    const GeoTagWriter* tagWriter = GeoTagWriter::recognizes(GeoTagWriter::QualifiedName(QString(), documentIdentifier));
    if (tagWriter) {
        GeoWriter writer;
        writer.setDocumentType(documentIdentifier);
        return writer.write(device, &document);
    } else {
        foreach(const auto &backend, s_backends) {
            if (backend.first == documentIdentifier) {
                backend.second->write(device, document);
                return true;
            }
        }

        mDebug() << "No writer registered to handle " << documentIdentifier;
        return false;
    }
}

bool GeoDataDocumentWriter::write(const QString &filename, const GeoDataDocument &document, const QString &documentIdentifier)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        mDebug() << "Cannot open" << filename << "for writing:" << file.errorString();
        return false;
    }

    QString const docType = documentIdentifier.isEmpty() ? determineDocumentIdentifier(filename) : documentIdentifier;
    return write(&file, document, docType);
}

void GeoDataDocumentWriter::registerWriter(GeoWriterBackend *writer, const QString &fileExtension)
{
    s_backends << QPair<QString, GeoWriterBackend*>(fileExtension, writer);
}

QString GeoDataDocumentWriter::determineDocumentIdentifier(const QString &filename)
{
    QString const fileExtension = QFileInfo(filename).suffix().toLower();
    if (fileExtension == QLatin1String("kml")) {
        return kml::kmlTag_nameSpaceOgc22;
    }
    if (fileExtension == QLatin1String("osm")) {
        return "0.6";
    }

    foreach(const auto &backend, s_backends) {
        if (backend.first == fileExtension) {
            return backend.first;
        }
    }

    qDebug() << "Unable to determine document from file extension " << fileExtension << ", falling back to KML document type";
    return kml::kmlTag_nameSpaceOgc22;
}

}
