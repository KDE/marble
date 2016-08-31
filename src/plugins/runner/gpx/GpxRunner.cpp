//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "GpxRunner.h"

#include "GeoDataDocument.h"
#include "GpxParser.h"
#include "MarbleDebug.h"

#include <QFile>

namespace Marble
{

GpxRunner::GpxRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

GpxRunner::~GpxRunner()
{
}

GeoDataDocument *GpxRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    QFile file( fileName );
    if ( !file.exists() ) {
        error = QStringLiteral("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    GpxParser parser;

    if ( !parser.read( &file ) ) {
        error = parser.errorString();
        mDebug() << error;
        return nullptr;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    GeoDataDocument* doc = static_cast<GeoDataDocument*>( document );
    doc->setDocumentRole( role );
    doc->setFileName( fileName );

    file.close();
    return doc;
}

}

#include "moc_GpxRunner.cpp"
