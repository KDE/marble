//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_PARSINGRUNNER_H
#define MARBLE_PARSINGRUNNER_H

#include <QObject>
#include "marble_export.h"

#include "GeoDataDocument.h"

namespace Marble
{

class MARBLE_EXPORT ParsingRunner : public QObject
{
    Q_OBJECT

public:
    explicit ParsingRunner( QObject *parent = 0 );

    /**
      * Start a file parsing.
      * If implemented in a plugin, make sure to include Parsing in the
      * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
      */
    virtual GeoDataDocument* parseFile( const QString &fileName, DocumentRole role, QString& error ) = 0;
};

}

#endif
