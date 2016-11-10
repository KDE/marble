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

#include "ParsingRunnerManager.h"

#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "PluginManager.h"
#include "ParseRunnerPlugin.h"
#include "RunnerTask.h"

#include <QFileInfo>
#include <QList>
#include <QThreadPool>
#include <QTimer>
#include <QMutex>

namespace Marble
{

class MarbleModel;

class Q_DECL_HIDDEN ParsingRunnerManager::Private
{
public:
    Private( ParsingRunnerManager *parent, const PluginManager *pluginManager );

    ~Private();

    void cleanupParsingTask();
    void addParsingResult(GeoDataDocument *document, const QString &error);

    ParsingRunnerManager *const q;
    const PluginManager *const m_pluginManager;
    QMutex m_parsingTasksMutex;
    int m_parsingTasks;
    GeoDataDocument *m_fileResult;
};

ParsingRunnerManager::Private::Private( ParsingRunnerManager *parent, const PluginManager *pluginManager ) :
    q( parent ),
    m_pluginManager( pluginManager ),
    m_parsingTasks(0),
    m_fileResult( 0 )
{
    qRegisterMetaType<GeoDataDocument*>( "GeoDataDocument*" );
}

ParsingRunnerManager::Private::~Private()
{
    // nothing to do
}

void ParsingRunnerManager::Private::cleanupParsingTask()
{
    QMutexLocker locker(&m_parsingTasksMutex);
    m_parsingTasks = qMax(0, m_parsingTasks-1);
    if (m_parsingTasks == 0) {
        emit q->parsingFinished();
    }
}

ParsingRunnerManager::ParsingRunnerManager( const PluginManager *pluginManager, QObject *parent ) :
    QObject( parent ),
    d( new Private( this, pluginManager ) )
{
    if ( QThreadPool::globalInstance()->maxThreadCount() < 4 ) {
        QThreadPool::globalInstance()->setMaxThreadCount( 4 );
    }
}

ParsingRunnerManager::~ParsingRunnerManager()
{
    delete d;
}

void ParsingRunnerManager::parseFile( const QString &fileName, DocumentRole role )
{
    QList<const ParseRunnerPlugin*> plugins = d->m_pluginManager->parsingRunnerPlugins();
    const QFileInfo fileInfo( fileName );
    const QString suffix = fileInfo.suffix().toLower();
    const QString completeSuffix = fileInfo.completeSuffix().toLower();

    d->m_parsingTasks = 0;
    foreach( const ParseRunnerPlugin *plugin, plugins ) {
        QStringList const extensions = plugin->fileExtensions();
        if ( extensions.isEmpty() || extensions.contains( suffix ) || extensions.contains( completeSuffix ) ) {
            ParsingTask *task = new ParsingTask( plugin->newRunner(), this, fileName, role );
            connect( task, SIGNAL(finished()), this, SLOT(cleanupParsingTask()) );
            mDebug() << "parse task " << plugin->nameId() << " " << (quintptr)task;
            ++d->m_parsingTasks;
            QThreadPool::globalInstance()->start( task );
        }
    }

    if (d->m_parsingTasks == 0) {
        emit parsingFinished();
    }
}

GeoDataDocument *ParsingRunnerManager::openFile( const QString &fileName, DocumentRole role, int timeout )
{
    d->m_fileResult = nullptr;
    QEventLoop localEventLoop;
    QTimer watchdog;
    watchdog.setSingleShot(true);
    connect( &watchdog, SIGNAL(timeout()),
             &localEventLoop, SLOT(quit()));
    connect(this, SIGNAL(parsingFinished()),
            &localEventLoop, SLOT(quit()), Qt::QueuedConnection );

    watchdog.start( timeout );
    parseFile( fileName, role);
    localEventLoop.exec();
    return d->m_fileResult;
}

void ParsingRunnerManager::Private::addParsingResult(GeoDataDocument *document, const QString &error)
{
    if ( document || !error.isEmpty() ) {
        if (document) {
            m_fileResult = document;
        }
        emit q->parsingFinished( document, error );
    }
}

}

#include "moc_ParsingRunnerManager.cpp"
