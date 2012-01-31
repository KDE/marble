//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MapQuestPlugin.h"
#include "MapQuestRunner.h"

#include "ui_MapQuestConfigWidget.h"

namespace Marble
{

MapQuestPlugin::MapQuestPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setName( tr( "MapQuest" ) );
    setNameId( "mapquest" );
    setDescription( tr( "Worldwide routing using mapquest.org" ) );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
    setGuiString( tr( "MapQuest Routing" ) );
}

MarbleAbstractRunner* MapQuestPlugin::newRunner() const
{
    return new MapQuestRunner;
}

class MapQuestConfigWidget : public RunnerPlugin::ConfigWidget
{
public:

    MapQuestConfigWidget()
        : RunnerPlugin::ConfigWidget()
    {
        ui_configWidget = new Ui::MapQuestConfigWidget;
        ui_configWidget->setupUi( this );

        ui_configWidget->preference->addItem( tr( "Car (fastest way)" ), "fastest" );
        ui_configWidget->preference->addItem( tr( "Car (shortest way)" ), "shortest" );
        ui_configWidget->preference->addItem( tr( "Pedestrian" ), "pedestrian" );
        ui_configWidget->preference->addItem( tr( "Bicycle" ), "bicycle" );
        ui_configWidget->preference->addItem( tr( "Transit (Public Transport)" ), "multimodal" );
    }

    virtual void loadSettings( const QHash<QString, QVariant> &settings_ )
    {
        QHash<QString, QVariant> settings = settings_;

        // Check if all fields are filled and fill them with default values.
        if ( !settings.contains( "preference" ) ) {
            settings.insert( "preference", "fastest" );
        }
        ui_configWidget->preference->setCurrentIndex(
                    ui_configWidget->preference->findData( settings.value( "preference" ).toString() ) );
        ui_configWidget->noMotorways->setCheckState( static_cast<Qt::CheckState>( settings.value( "noMotorways" ).toInt() ) );
        ui_configWidget->noTollways->setCheckState( static_cast<Qt::CheckState>( settings.value( "noTollways" ).toInt() ) );
        ui_configWidget->noFerries->setCheckState( static_cast<Qt::CheckState>( settings.value( "noFerries" ).toInt() ) );
    }

    virtual QHash<QString, QVariant> settings() const
    {
        QHash<QString,QVariant> settings;
        settings.insert( "preference",
                         ui_configWidget->preference->itemData( ui_configWidget->preference->currentIndex() ) );
        settings.insert( "noMotorways", ui_configWidget->noMotorways->checkState() );
        settings.insert( "noTollways", ui_configWidget->noTollways->checkState() );
        settings.insert( "noFerries", ui_configWidget->noFerries->checkState() );
        return settings;
    }
private:
    Ui::MapQuestConfigWidget *ui_configWidget;
};

RunnerPlugin::ConfigWidget *MapQuestPlugin::configWidget()
{
    return new MapQuestConfigWidget();
}


bool MapQuestPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    QSet<RoutingProfilesModel::ProfileTemplate> availableTemplates;
    availableTemplates.insert( RoutingProfilesModel::CarFastestTemplate );
    availableTemplates.insert( RoutingProfilesModel::CarShortestTemplate );
    availableTemplates.insert( RoutingProfilesModel::BicycleTemplate );
    availableTemplates.insert( RoutingProfilesModel::PedestrianTemplate );
    return availableTemplates.contains( profileTemplate );
}

QHash< QString, QVariant > MapQuestPlugin::templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    QHash<QString, QVariant> result;
    switch ( profileTemplate ) {
    case RoutingProfilesModel::CarFastestTemplate:
        result["preference"] = "fastest";
        break;
    case RoutingProfilesModel::CarShortestTemplate:
        result["preference"] = "shortest";
        break;
    case RoutingProfilesModel::CarEcologicalTemplate:
        break;
    case RoutingProfilesModel::BicycleTemplate:
        result["preference"] = "bicycle";
        break;
    case RoutingProfilesModel::PedestrianTemplate:
        result["preference"] = "pedestrian";
        break;
    case RoutingProfilesModel::LastTemplate:
        Q_ASSERT( false );
        break;
    }
    return result;
}

}

Q_EXPORT_PLUGIN2( MapQuestPlugin, Marble::MapQuestPlugin )

#include "MapQuestPlugin.moc"
