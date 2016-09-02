/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2013 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "JsonRunner.h"
#include "JsonParser.h"

#include "GeoDataDocument.h"
#include "MarbleDebug.h"

#include <QFile>

namespace Marble
{

JsonRunner::JsonRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

JsonRunner::~JsonRunner()
{
}

GeoDataDocument *JsonRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    // Check file exists
    QFile file( fileName );
    if ( !file.exists() ) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    // Create parser
    JsonParser parser;

    // Start parsing
    if ( !parser.read( &file ) ) {
        error = QStringLiteral("Could not parse GeoJSON from %1").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    GeoDataDocument* document = parser.releaseDocument();
    file.close();
    document->setDocumentRole( role );
    document->setFileName( fileName );

    return document;
}

}

#include "moc_JsonRunner.cpp"
