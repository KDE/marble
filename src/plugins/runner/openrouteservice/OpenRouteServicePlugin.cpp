//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "OpenRouteServicePlugin.h"
#include "OpenRouteServiceRunner.h"

#include "ui_OpenRouteServiceConfigWidget.h"

namespace Marble
{

OpenRouteServicePlugin::OpenRouteServicePlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
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
    return QStringLiteral("openrouteservice");
}

QString OpenRouteServicePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString OpenRouteServicePlugin::description() const
{
    return tr( "Routing in Europe using openrouteservice.org" );
}

QString OpenRouteServicePlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QVector<PluginAuthor> OpenRouteServicePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

RoutingRunner *OpenRouteServicePlugin::newRunner() const
{
    return new OpenRouteServiceRunner;
}

class OpenRouteServiceConfigWidget : public RoutingRunnerPlugin::ConfigWidget
{
    Q_OBJECT

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
        ui_configWidget->preference->addItem( tr( "Bicycle (preferred Cycleway/-route)" ), "BicycleTour" );
    }

    virtual void loadSettings( const QHash<QString, QVariant> &settings_ )
    {
        QHash<QString, QVariant> settings = settings_;

        // Check if all fields are filled and fill them with default values.
        if (!settings.contains(QStringLiteral("preference"))) {
            settings.insert(QStringLiteral("preference"), QStringLiteral("Fastest"));
        }
        ui_configWidget->preference->setCurrentIndex(
            ui_configWidget->preference->findData(settings.value(QStringLiteral("preference")).toString()));
        ui_configWidget->noMotorways->setCheckState(static_cast<Qt::CheckState>(settings.value(QStringLiteral("noMotorways")).toInt()));
        ui_configWidget->noTollways->setCheckState(static_cast<Qt::CheckState>(settings.value(QStringLiteral("noTollways")).toInt()));
        ui_configWidget->noFerries->setCheckState(static_cast<Qt::CheckState>(settings.value(QStringLiteral("noFerries")).toInt()));
    }

    virtual QHash<QString, QVariant> settings() const
    {
        QHash<QString,QVariant> settings;
        settings.insert(QStringLiteral("preference"),
                        ui_configWidget->preference->itemData( ui_configWidget->preference->currentIndex() ) );
        settings.insert(QStringLiteral("noMotorways"), ui_configWidget->noMotorways->checkState());
        settings.insert(QStringLiteral("noTollways"), ui_configWidget->noTollways->checkState());
        settings.insert(QStringLiteral("noFerries"), ui_configWidget->noFerries->checkState());
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
            result.insert(QStringLiteral("preference"), QStringLiteral("Fastest"));
            break;
        case RoutingProfilesModel::CarShortestTemplate:
            result.insert(QStringLiteral("preference"), QStringLiteral("Shortest"));
            break;
        case RoutingProfilesModel::CarEcologicalTemplate:
            break;
        case RoutingProfilesModel::BicycleTemplate:
            result.insert(QStringLiteral("preference"), QStringLiteral("Bicycle"));
            break;
        case RoutingProfilesModel::PedestrianTemplate:
            result.insert(QStringLiteral("preference"), QStringLiteral("Pedestrian"));
            break;
        case RoutingProfilesModel::LastTemplate:
            Q_ASSERT( false );
            break;
    }
    return result;
}

}

#include "OpenRouteServicePlugin.moc" // needed for Q_OBJECT here in source
