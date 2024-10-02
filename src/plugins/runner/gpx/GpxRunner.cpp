// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "GpxRunner.h"

#include "GeoDataDocument.h"
#include "GpxParser.h"
#include "MarbleDebug.h"

#include <QFile>

namespace Marble
{

GpxRunner::GpxRunner(QObject *parent)
    : ParsingRunner(parent)
{
}

GpxRunner::~GpxRunner() = default;

GeoDataDocument *GpxRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    QFile file(fileName);
    if (!file.exists()) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Open file in right mode
    file.open(QIODevice::ReadOnly);

    GpxParser parser;

    if (!parser.read(&file)) {
        error = parser.errorString();
        mDebug() << error;
        return nullptr;
    }
    GeoDocument *document = parser.releaseDocument();
    Q_ASSERT(document);
    auto doc = static_cast<GeoDataDocument *>(document);
    doc->setDocumentRole(role);
    doc->setFileName(fileName);

    file.close();
    return doc;
}

}

#include "moc_GpxRunner.cpp"
