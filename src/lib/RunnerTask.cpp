#include "RunnerTask.h"

#include "MarbleAbstractRunner.h"
#include "MarbleDebug.h"
#include "routing/RouteSkeleton.h"

#include <QtCore/QTimer>

namespace Marble
{

RunnerTask::RunnerTask( MarbleAbstractRunner* runner ) :
        m_runner( runner )
{
    // nothing to do
}

void RunnerTask::run()
{
    QTimer watchdog;
    watchdog.setSingleShot( true );
    QEventLoop localEventLoop;
    QObject::connect( &watchdog, SIGNAL( timeout() ), &localEventLoop, SLOT( quit() ) );
    runTask( &localEventLoop );
    watchdog.start( 30 * 1000 );
    localEventLoop.exec();

    if( watchdog.isActive() ) {
        watchdog.stop(); // completed within timeout
    } else {
        mDebug() << "Timeout reached while waiting for result. Killing the runner.";
        /** @todo: Report back to MarbleRunnerManager to announce finish without
          * results, if needed
          */
        runner()->deleteLater();
    }
}

MarbleAbstractRunner* RunnerTask::runner()
{
    return m_runner;
}

SearchTask::SearchTask(MarbleAbstractRunner* runner, const QString &searchTerm) :
        RunnerTask( runner ), m_searchTerm( searchTerm )
{
    // nothing to do
}

void SearchTask::runTask( QEventLoop *localEventLoop )
{
    QObject::connect( runner(), SIGNAL( searchFinished( QVector<GeoDataPlacemark*> ) ),
            localEventLoop, SLOT( quit() ) );
    runner()->search( m_searchTerm );
}

ReverseGeocodingTask::ReverseGeocodingTask( MarbleAbstractRunner* runner, const GeoDataCoordinates &coordinates ) :
        RunnerTask( runner ), m_coordinates( coordinates )
{
    // nothing to do
}

void ReverseGeocodingTask::runTask( QEventLoop *localEventLoop )
{
    QObject::connect( runner(), SIGNAL( reverseGeocodingFinished( GeoDataPlacemark) ),
            localEventLoop, SLOT( quit() ) );
    runner()->reverseGeocoding( m_coordinates );
}

RoutingTask::RoutingTask( MarbleAbstractRunner* runner, RouteSkeleton* routeSkeleton ) :
        RunnerTask( runner ), m_routeSkeleton( routeSkeleton )
{
    // nothing to do
}

void RoutingTask::runTask( QEventLoop *localEventLoop )
{
    QObject::connect( runner(), SIGNAL( routeCalculated( GeoDataDocument*) ),
            localEventLoop, SLOT( quit() ) );
    runner()->retrieveRoute( m_routeSkeleton );
}

}
