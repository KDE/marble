// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "KmlRunner.h"

#include "GeoDataDocument.h"
#include "KmlParser.h"
#include "MarbleDebug.h"
#include <MarbleZipReader.h>

#include <QBuffer>
#include <QFile>
#include <QFileInfo>

namespace Marble
{

KmlRunner::KmlRunner(QObject *parent)
    : ParsingRunner(parent)
{
}

KmlRunner::~KmlRunner()
{
}

GeoDataDocument *KmlRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        error = QStringLiteral("Cannot open file %1").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    QBuffer buffer;
    QIODevice *device = nullptr;

    if (fileName.toLower().endsWith(QLatin1StringView(".kmz"))) {
        MarbleZipReader zipReader(&file);

        QStringList kmlFiles;
        for (const MarbleZipReader::FileInfo &zipFileInfo : zipReader.fileInfoList()) {
            if (zipFileInfo.filePath.toLower().endsWith(QLatin1StringView(".kml"))) {
                kmlFiles.append(zipFileInfo.filePath);
            }
        }
        if (kmlFiles.empty()) {
            error = QStringLiteral("File %1 does not contain any KML files").arg(fileName);
            mDebug() << error;
            return nullptr;
        } else if (kmlFiles.size() > 1) {
            mDebug() << QStringLiteral("File %1 contains multiple KML files").arg(fileName);
        }

        QByteArray const data = zipReader.fileData(kmlFiles[0]);
        buffer.setData(data);
        buffer.open(QBuffer::ReadOnly);
        device = &buffer;
    } else {
        device = &file;
    }

    KmlParser parser;
    if (!parser.read(device)) {
        error = parser.errorString();
        mDebug() << error;
        return nullptr;
    }

    GeoDocument *document = parser.releaseDocument();
    Q_ASSERT(document);
    GeoDataDocument *doc = static_cast<GeoDataDocument *>(document);
    doc->setDocumentRole(role);
    doc->setFileName(fileName);

    file.close();
    return doc;
}

}

#include "moc_KmlRunner.cpp"
