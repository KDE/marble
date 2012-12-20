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

MapQuestPlugin::MapQuestPlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString MapQuestPlugin::name() const
{
    return tr( "MapQuest Routing" );
}

QString MapQuestPlugin::guiString() const
{
    return tr( "MapQuest" );
}

QString MapQuestPlugin::nameId() const
{
    return "mapquest";
}

QString MapQuestPlugin::version() const
{
    return "1.0";
}

QString MapQuestPlugin::description() const
{
    return tr( "Worldwide routing using mapquest.org" );
}

QString MapQuestPlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> MapQuestPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

RoutingRunner *MapQuestPlugin::newRunner() const
{
    return new MapQuestRunner;
}

class MapQuestConfigWidget : public RoutingRunnerPlugin::ConfigWidget
{
public:

    MapQuestConfigWidget()
        : RoutingRunnerPlugin::ConfigWidget()
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

RoutingRunnerPlugin::ConfigWidget *MapQuestPlugin::configWidget()
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
