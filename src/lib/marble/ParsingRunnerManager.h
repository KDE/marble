// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

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
    explicit ParsingRunnerManager( const PluginManager *pluginManager, QObject *parent = nullptr );

    ~ParsingRunnerManager() override;

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
