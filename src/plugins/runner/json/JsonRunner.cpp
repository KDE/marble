/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2013 Ander Pijoan <ander.pijoan@deusto.es>
    SPDX-FileCopyrightText: 2019 John Zaitseff <J.Zaitseff@zap.org.au>
*/

#include "JsonRunner.h"
#include "JsonParser.h"

#include "GeoDataDocument.h"
#include "MarbleDebug.h"

#include <QFile>

namespace Marble
{

JsonRunner::JsonRunner(QObject *parent)
    : ParsingRunner(parent)
{
}

JsonRunner::~JsonRunner() = default;

GeoDataDocument *JsonRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    // Check that the file exists
    QFile file(fileName);
    if (!file.exists()) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Open file in the correct mode
    file.open(QIODevice::ReadOnly);

    // Create parser
    JsonParser parser;

    // Start parsing
    if (!parser.read(&file)) {
        error = QStringLiteral("Could not parse GeoJSON from %1").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    GeoDataDocument *document = parser.releaseDocument();
    file.close();

    document->setDocumentRole(role);
    document->setFileName(fileName);

    return document;
}

}

#include "moc_JsonRunner.cpp"
