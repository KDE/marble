// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#include "GeoDataDocumentWriter.h"

#include "GeoDataDocument.h"
#include "GeoTagWriter.h"
#include "GeoWriter.h"
#include "GeoWriterBackend.h"
#include "KmlElementDictionary.h"

#include <MarbleDebug.h>
#include <QFileInfo>

namespace Marble
{

QSet<QPair<QString, GeoWriterBackend *>> GeoDataDocumentWriter::s_backends;

bool GeoDataDocumentWriter::write(QIODevice *device, const GeoDataDocument &document, const QString &documentIdentifier)
{
    const GeoTagWriter *tagWriter = GeoTagWriter::recognizes(GeoTagWriter::QualifiedName(QString(), documentIdentifier));
    if (tagWriter) {
        GeoWriter writer;
        writer.setDocumentType(documentIdentifier);
        return writer.write(device, &document);
    } else {
        for (const auto &backend : s_backends) {
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
    s_backends << QPair<QString, GeoWriterBackend *>(fileExtension, writer);
}

void GeoDataDocumentWriter::unregisterWriter(GeoWriterBackend *writer, const QString &fileExtension)
{
    auto pair = QPair<QString, GeoWriterBackend *>(fileExtension, writer);
    s_backends.remove(pair);
    delete writer;
}

QString GeoDataDocumentWriter::determineDocumentIdentifier(const QString &filename)
{
    QString const fileExtension = QFileInfo(filename).suffix().toLower();
    if (fileExtension == QLatin1StringView("kml")) {
        return QString::fromLatin1(kml::kmlTag_nameSpaceOgc22);
    }
    if (fileExtension == QLatin1StringView("osm")) {
        return QStringLiteral("0.6");
    }

    for (const auto &backend : s_backends) {
        if (backend.first == fileExtension) {
            return backend.first;
        }
    }

    qDebug() << "Unable to determine document from file extension " << fileExtension << ", falling back to KML document type";
    return QString::fromLatin1(kml::kmlTag_nameSpaceOgc22);
}

}
