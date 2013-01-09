//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_PARSINGRUNNER_H
#define MARBLE_PARSINGRUNNER_H

#include <QtCore/QObject>
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
      * Start a file parsing. Called by MarbleRunnerManager, runners
      * are expected to return the result via the parsingFinished signal.
      * If implemented in a plugin, make sure to include Parsing in the
      * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
      */
    virtual void parseFile( const QString &fileName, DocumentRole role ) = 0;

Q_SIGNALS:
    /**
     * File parsing is finished, result in the given document object.
     * The signal should be emitted with null document and error description in case of fault.
     * To be emitted by runners after a @see parseFile call.
     */
    void parsingFinished( GeoDataDocument* document, const QString& error = QString() );
};

}

#endif
