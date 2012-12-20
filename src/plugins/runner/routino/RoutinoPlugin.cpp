//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutinoPlugin.h"
#include "RoutinoRunner.h"
#include "MarbleDirs.h"

#include "ui_RoutinoConfigWidget.h"

namespace Marble
{

RoutinoPlugin::RoutinoPlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
}

QString RoutinoPlugin::name() const
{
    return tr( "Routino Routing" );
}

QString RoutinoPlugin::guiString() const
{
    return tr( "Routino" );
}

QString RoutinoPlugin::nameId() const
{
    return "routino";
}

QString RoutinoPlugin::version() const
{
    return "1.0";
}

QString RoutinoPlugin::description() const
{
    return tr( "Retrieves routes from routino" );
}

QString RoutinoPlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> RoutinoPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

RoutingRunner *RoutinoPlugin::newRunner() const
{
    return new RoutinoRunner;
}

class RoutinoConfigWidget : public RoutingRunnerPlugin::ConfigWidget
{
public:
    RoutinoConfigWidget()
        : RoutingRunnerPlugin::ConfigWidget()
    {
        ui_configWidget = new Ui::RoutinoConfigWidget;
        ui_configWidget->setupUi( this );
        QStringList transports;
        //TODO: read from profiles.xml
        //TODO: translate
        transports << "foot" << "horse" << "wheelchair" << "bicycle" << "moped" << "motorbike" << "motorcar" << "goods" << "hgv" << "psv";
        foreach ( const QString &transport, transports) {
            ui_configWidget->transport->addItem(transport, transport);
        }
    }
    virtual ~RoutinoConfigWidget()
    {
        delete ui_configWidget;
    }
    virtual void loadSettings( const QHash<QString, QVariant> &settings_ )
    {
        QHash<QString, QVariant> settings = settings_;

        // Check if all fields are filled and fill them with default values.
        if ( !settings.contains( "transport" ) ) {
            settings.insert( "transport", "motorcar" );
        }
        ui_configWidget->transport->setCurrentIndex(
            ui_configWidget->transport->findData( settings.value( "transport" ).toString() ) );
        if ( settings.value( "method" ).toString() == "shortest" ) {
            ui_configWidget->shortest->setChecked( true );
        } else {
            ui_configWidget->fastest->setChecked( true );
        }
    }

    virtual QHash<QString, QVariant> settings() const
    {
        QHash<QString,QVariant> settings;
        settings.insert( "transport",
                        ui_configWidget->transport->itemData( ui_configWidget->transport->currentIndex() ) );

        if ( ui_configWidget->shortest->isChecked() ) {
            settings.insert( "method", "shortest" );
        } else {
            settings.insert( "method", "fastest" );
        }
        return settings;
    }
private:
    Ui::RoutinoConfigWidget *ui_configWidget;
};

RoutingRunnerPlugin::ConfigWidget *RoutinoPlugin::configWidget()
{
    return new RoutinoConfigWidget();
}

bool RoutinoPlugin::supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const
{
    QSet<RoutingProfilesModel::ProfileTemplate> availableTemplates;
        availableTemplates.insert( RoutingProfilesModel::CarFastestTemplate );
        availableTemplates.insert( RoutingProfilesModel::CarShortestTemplate );
        availableTemplates.insert( RoutingProfilesModel::BicycleTemplate );
        availableTemplates.insert( RoutingProfilesModel::PedestrianTemplate );
    return availableTemplates.contains( profileTemplate );
}

QHash< QString, QVariant > RoutinoPlugin::templateSettings(RoutingProfilesModel::ProfileTemplate profileTemplate) const
{
    QHash<QString, QVariant> result;
    switch ( profileTemplate ) {
        case RoutingProfilesModel::CarFastestTemplate:
            result["transport"] = "motorcar";
            result["method"] = "fastest";
            break;
        case RoutingProfilesModel::CarShortestTemplate:
            result["transport"] = "motorcar";
            result["method"] = "shortest";
            break;
        case RoutingProfilesModel::CarEcologicalTemplate:
            break;
        case RoutingProfilesModel::BicycleTemplate:
            result["transport"] = "bicycle";
            result["method"] = "shortest";
            break;
        case RoutingProfilesModel::PedestrianTemplate:
            result["transport"] = "foot";
            result["method"] = "shortest";
            break;
        case RoutingProfilesModel::LastTemplate:
            Q_ASSERT( false );
            break;
    }
    return result;
}

bool RoutinoPlugin::canWork() const
{
    QDir mapDir = QDir( MarbleDirs::localPath() + "/maps/earth/routino/" );
    return mapDir.exists();
}

}

Q_EXPORT_PLUGIN2( RoutinoPlugin, Marble::RoutinoPlugin )

#include "RoutinoPlugin.moc"
