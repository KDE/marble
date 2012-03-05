#include "RunnerPlugin.h"

namespace Marble
{

class RunnerPluginPrivate
{
public:
    RunnerPlugin::Capabilities m_capabilities;

    QStringList m_supportedCelestialBodies;

    bool m_canWorkOffline;

    QString m_statusMessage;

    RunnerPluginPrivate();
};

RunnerPluginPrivate::RunnerPluginPrivate() : m_canWorkOffline( true )
{
    // nothing to do
}

RunnerPlugin::RunnerPlugin( QObject* parent ) : QObject( parent ),
    d( new RunnerPluginPrivate )
{
    setCapabilities( RunnerPlugin::None );
}

RunnerPlugin::~RunnerPlugin()
{
    delete d;
}

RunnerPlugin::Capabilities RunnerPlugin::capabilities() const
{
    return d->m_capabilities;
}

bool RunnerPlugin::supports( Capability capability ) const
{
    return d->m_capabilities & capability;
}

void RunnerPlugin::setCapabilities( Capabilities capabilities )
{
    d->m_capabilities = capabilities;
}

QIcon RunnerPlugin::icon() const
{
    return QIcon();
}

void RunnerPlugin::initialize()
{
    // nothing to do
}

bool RunnerPlugin::isInitialized() const
{
    return true;
}

bool RunnerPlugin::supportsCelestialBody( const QString &celestialBodyId ) const
{
    if ( d->m_supportedCelestialBodies.isEmpty() ) {
        return true;
    }

    return d->m_supportedCelestialBodies.contains( celestialBodyId );
}

void RunnerPlugin::setSupportedCelestialBodies( const QStringList &celestialBodies )
{
    d->m_supportedCelestialBodies = celestialBodies;
}

void RunnerPlugin::setCanWorkOffline( bool canWorkOffline )
{
    d->m_canWorkOffline = canWorkOffline;
}

bool RunnerPlugin::canWorkOffline() const
{
    return d->m_canWorkOffline;
}

RunnerPlugin::ConfigWidget *RunnerPlugin::configWidget()
{
    return 0;
}

bool RunnerPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate ) const
{
    return false;
}

QHash< QString, QVariant > RunnerPlugin::templateSettings( RoutingProfilesModel::ProfileTemplate ) const
{
    return QHash< QString, QVariant >();
}

bool RunnerPlugin::canWork( Capability capability ) const
{
    return supports( capability );
}

QString RunnerPlugin::statusMessage() const
{
    return d->m_statusMessage;
}

void RunnerPlugin::setStatusMessage( const QString &message )
{
    d->m_statusMessage = message;
}

}

#include "RunnerPlugin.moc"
