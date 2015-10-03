//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "OsmRunner.h"

#include "GeoDataDocument.h"
#include "OsmParser.h"
#include "MarbleDebug.h"

#include <QFile>

namespace Marble
{

OsmRunner::OsmRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

OsmRunner::~OsmRunner()
{
}

GeoDataDocument *OsmRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    QFile  file( fileName );
    if ( !file.exists() ) {
        error = QString("File %1 does not exist").arg(fileName);
        mDebug() << error;
        return nullptr;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    OsmParser parser;

    if ( !parser.read( &file ) ) {
        error = parser.errorString();
        mDebug() << error;
        return nullptr;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );
    GeoDataDocument* doc = static_cast<GeoDataDocument*>( document );
    parser.adjustStyles(doc);
    doc->setDocumentRole( role );
    doc->setFileName( fileName );

    file.close();
    return doc;
}

}

#include "moc_OsmRunner.cpp"
