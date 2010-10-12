#include "RunnerPlugin.h"

namespace Marble
{

class RunnerPluginPrivate
{
public:
    RunnerPlugin::Capabilities m_capabilities;

    QStringList m_supportedCelestialBodies;

    bool m_canWorkOffline;

    QString m_name;

    QString m_guiString;

    QString m_nameId;

    QString m_description;

    QIcon m_icon;

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
    setName( "Plugin author forgot to call setName()" );
    setNameId( "Plugin author forgot to call setNameId()" );
    setGuiString( "Plugin author forgot to call setGuiString()" );
    setDescription( "Plugin author forgot to call setDescription()" );
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

QString RunnerPlugin::name() const
{
    return d->m_name;
}

QString RunnerPlugin::guiString() const
{
    return d->m_guiString;
}

QString RunnerPlugin::nameId() const
{
    return d->m_nameId;
}

QString RunnerPlugin::description() const
{
    return d->m_description;
}

QIcon RunnerPlugin::icon() const
{
    return d->m_icon;
}

void RunnerPlugin::initialize()
{
    // nothing to do
}

bool RunnerPlugin::isInitialized() const
{
    return true;
}

void RunnerPlugin::setName( const QString &name )
{
    d->m_name = name;
}

void RunnerPlugin::setGuiString( const QString &guiString )
{
    d->m_guiString = guiString;
}

void RunnerPlugin::setNameId( const QString &nameId )
{
    d->m_nameId = nameId;
}

void RunnerPlugin::setDescription( const QString &description )
{
    d->m_description = description;
}

void RunnerPlugin::setIcon( const QIcon &icon )
{
    d->m_icon = icon;
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

RunnerPlugin::ConfigWidget *RunnerPlugin::configWidget() const
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

}

#include "RunnerPlugin.moc"
