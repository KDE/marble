//
// This file is part of the Marble Desktop Globe.
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

RoutinoPlugin::RoutinoPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Routino" ) );
    setNameId( "routino" );
    setDescription( tr( "Retrieves routes from routino" ) );
    setGuiString( tr( "Routino Routing" ) );
}

MarbleAbstractRunner* RoutinoPlugin::newRunner() const
{
    return new RoutinoRunner;
}

class RoutinoConfigWidget : public RunnerPlugin::ConfigWidget
{
public:
    RoutinoConfigWidget()
        : RunnerPlugin::ConfigWidget()
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

RunnerPlugin::ConfigWidget *RoutinoPlugin::configWidget()
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

bool RoutinoPlugin::canWork( Capability capability ) const
{
    if ( supports( capability ) ) {
        QDir mapDir = QDir( MarbleDirs::localPath() + "/maps/earth/routino/" );
        return mapDir.exists();
    } else {
        return false;
    }
}

}

Q_EXPORT_PLUGIN2( RoutinoPlugin, Marble::RoutinoPlugin )

#include "RoutinoPlugin.moc"
