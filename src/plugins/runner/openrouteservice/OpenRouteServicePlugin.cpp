//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OpenRouteServicePlugin.h"
#include "OpenRouteServiceRunner.h"

#include "ui_OpenRouteServiceConfigWidget.h"

namespace Marble
{

OpenRouteServicePlugin::OpenRouteServicePlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString OpenRouteServicePlugin::name() const
{
    return tr( "OpenRouteService Routing" );
}

QString OpenRouteServicePlugin::guiString() const
{
    return tr( "OpenRouteService" );
}

QString OpenRouteServicePlugin::nameId() const
{
    return "openrouteservice";
}

QString OpenRouteServicePlugin::version() const
{
    return "1.0";
}

QString OpenRouteServicePlugin::description() const
{
    return tr( "Routing in Europe using openrouteservice.org" );
}

QString OpenRouteServicePlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> OpenRouteServicePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

RoutingRunner *OpenRouteServicePlugin::newRunner() const
{
    return new OpenRouteServiceRunner;
}

class OpenRouteServiceConfigWidget : public RoutingRunnerPlugin::ConfigWidget
{
public:

    OpenRouteServiceConfigWidget()
        : RoutingRunnerPlugin::ConfigWidget()
    {
        ui_configWidget = new Ui::OpenRouteServiceConfigWidget;
        ui_configWidget->setupUi( this );

        ui_configWidget->preference->addItem( tr( "Car (fastest way)" ), "Fastest" );
        ui_configWidget->preference->addItem( tr( "Car (shortest way)" ), "Shortest" );
        ui_configWidget->preference->addItem( tr( "Pedestrian (shortest way)" ), "Pedestrian" );
        ui_configWidget->preference->addItem( tr( "Bicycle (shortest track)" ), "Bicycle" );
        ui_configWidget->preference->addItem( tr( "Bicycle (Mountainbike)" ), "BicycleMTB" );
        ui_configWidget->preference->addItem( tr( "Bicycle (Racer)" ), "BicycleRacer" );
        ui_configWidget->preference->addItem( tr( "Bicycle (safest track)" ), "BicycleSafety" );
        ui_configWidget->preference->addItem( tr( "Bicycle (preferred Cycleway/-route)" ), "BicycleRoute" );
    }

    virtual void loadSettings( const QHash<QString, QVariant> &settings_ )
    {
        QHash<QString, QVariant> settings = settings_;

        // Check if all fields are filled and fill them with default values.
        if ( !settings.contains( "preference" ) ) {
            settings.insert( "preference", "Fastest" );
        }
        ui_configWidget->preference->setCurrentIndex(
            ui_configWidget->preference->findData( settings.value( "preference" ).toString() ) );
        ui_configWidget->noMotorways->setCheckState( static_cast<Qt::CheckState>( settings.value( "noMotorways" ).toInt() ) );
        ui_configWidget->noTollways->setCheckState( static_cast<Qt::CheckState>( settings.value( "noTollways" ).toInt() ) );
    }

    virtual QHash<QString, QVariant> settings() const
    {
        QHash<QString,QVariant> settings;
        settings.insert( "preference",
                        ui_configWidget->preference->itemData( ui_configWidget->preference->currentIndex() ) );
        settings.insert( "noMotorways", ui_configWidget->noMotorways->checkState() );
        settings.insert( "noTollways", ui_configWidget->noTollways->checkState() );
        return settings;
    }
private:
    Ui::OpenRouteServiceConfigWidget *ui_configWidget;
};

RoutingRunnerPlugin::ConfigWidget *OpenRouteServicePlugin::configWidget()
{
    return new OpenRouteServiceConfigWidget();
}


bool OpenRouteServicePlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    QSet<RoutingProfilesModel::ProfileTemplate> availableTemplates;
        availableTemplates.insert( RoutingProfilesModel::CarFastestTemplate );
        availableTemplates.insert( RoutingProfilesModel::CarShortestTemplate );
        availableTemplates.insert( RoutingProfilesModel::BicycleTemplate );
        availableTemplates.insert( RoutingProfilesModel::PedestrianTemplate );
    return availableTemplates.contains( profileTemplate );
}

QHash< QString, QVariant > OpenRouteServicePlugin::templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    QHash<QString, QVariant> result;
    switch ( profileTemplate ) {
        case RoutingProfilesModel::CarFastestTemplate:
            result["preference"] = "Fastest";
            break;
        case RoutingProfilesModel::CarShortestTemplate:
            result["preference"] = "Shortest";
            break;
        case RoutingProfilesModel::CarEcologicalTemplate:
            break;
        case RoutingProfilesModel::BicycleTemplate:
            result["preference"] = "Bicycle";
            break;
        case RoutingProfilesModel::PedestrianTemplate:
            result["preference"] = "Pedestrian";
            break;
        case RoutingProfilesModel::LastTemplate:
            Q_ASSERT( false );
            break;
    }
    return result;
}

}

Q_EXPORT_PLUGIN2( OpenRouteServicePlugin, Marble::OpenRouteServicePlugin )

#include "OpenRouteServicePlugin.moc"
