//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLE_PARSINGRUNNERMANAGER_H
#define MARBLE_PARSINGRUNNERMANAGER_H

#include <QObject>

#include "marble_export.h"

#include "GeoDataDocument.h"

namespace Marble
{

class PluginManager;

class MARBLE_EXPORT ParsingRunnerManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param pluginManager The plugin manager that gives access to RunnerPlugins
     * @param parent Optional parent object
     */
    explicit ParsingRunnerManager( const PluginManager *pluginManager, QObject *parent = 0 );

    ~ParsingRunnerManager();

    /**
     * Parse the file using the runners for various formats
     * @see parseFile is asynchronous with results returned using the
     * @see parsingFinished signal.
     * @see openFile is blocking.
     * @see parsingFinished signal indicates all runners are finished.
     */
    void parseFile( const QString &fileName, DocumentRole role = UserDocument );
    GeoDataDocument *openFile( const QString &fileName, DocumentRole role = UserDocument, int timeout = 30000 );

Q_SIGNALS:
    /**
     * The file was parsed and potential error message
     */
    void parsingFinished( GeoDataDocument *document, const QString &error = QString() );

    /**
     * Emitted whenever all runners are finished for the query
     */
    void parsingFinished();

private:
    Q_PRIVATE_SLOT( d, void cleanupParsingTask() )
    Q_PRIVATE_SLOT( d, void addParsingResult( GeoDataDocument *document, const QString &error ) )

    class Private;
    friend class Private;
    Private *const d;
};

}

#endif
