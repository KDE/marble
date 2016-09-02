//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mihail Ivchenko <ematirov@gmail.com>
//

#include "CycleStreetsPlugin.h"
#include "CycleStreetsRunner.h"
#include "MarbleGlobal.h"
#include "MarbleLocale.h"

#include "ui_CycleStreetsConfigWidget.h"

namespace Marble
{

class CycleStreetsConfigWidget : public RoutingRunnerPlugin::ConfigWidget
{
    Q_OBJECT

public:
    CycleStreetsConfigWidget();
    virtual void loadSettings( const QHash<QString, QVariant> &settings );
    virtual QHash<QString, QVariant> settings() const;

private:
    Ui::CycleStreetsConfigWidget *ui_configWidget;
};

CycleStreetsConfigWidget::CycleStreetsConfigWidget()
    : RoutingRunnerPlugin::ConfigWidget(),
      ui_configWidget( new Ui::CycleStreetsConfigWidget )
{
    ui_configWidget->setupUi( this );

    ui_configWidget->plan->addItem( tr( "balanced" ), "balanced" );
    ui_configWidget->plan->addItem( tr( "fastest" ), "fastest" );
    ui_configWidget->plan->addItem( tr( "quietest" ), "quietest" );
    ui_configWidget->plan->addItem( tr( "shortest" ), "shortest" );

    if ( MarbleGlobal::getInstance()->locale()->measurementSystem() == MarbleLocale::MetricSystem ) {
        ui_configWidget->speed->addItem( tr( "slow (16 km/h)" ), "16" );
        ui_configWidget->speed->addItem( tr( "normal (20 km/h)" ), "20" );
        ui_configWidget->speed->addItem( tr( "fast (24 km/h)") , "24" );
    } else {
        ui_configWidget->speed->addItem( tr( "slow (10 mph)" ), "16" );
        ui_configWidget->speed->addItem( tr( "normal (12 mph)" ), "20" );
        ui_configWidget->speed->addItem( tr( "fast (15 mph)") , "24" );
    }
}

void CycleStreetsConfigWidget::loadSettings( const QHash<QString, QVariant> &settings_ )
{
    QHash<QString, QVariant> settings = settings_;

    // Check if all fields are filled and fill them with default values.
    if (!settings.contains(QStringLiteral("plan"))) {
        settings.insert(QStringLiteral("plan"), QStringLiteral("balanced"));
    }
    if (!settings.contains(QStringLiteral("speed"))) {
        settings.insert(QStringLiteral("speed"), QStringLiteral("20"));
    }
    ui_configWidget->plan->setCurrentIndex(
                ui_configWidget->plan->findData(settings.value(QStringLiteral("plan"))));
    ui_configWidget->speed->setCurrentIndex(
                ui_configWidget->speed->findData(settings.value(QStringLiteral("speed"))));
}

QHash<QString, QVariant> CycleStreetsConfigWidget::settings() const
{
    QHash<QString,QVariant> settings;
    settings.insert(QStringLiteral("plan"),
                     ui_configWidget->plan->itemData( ui_configWidget->plan->currentIndex() ) );
    settings.insert(QStringLiteral("speed"),
                     ui_configWidget->speed->itemData( ui_configWidget->speed->currentIndex() ) );
    return settings;
}

CycleStreetsPlugin::CycleStreetsPlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( false );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString CycleStreetsPlugin::name() const
{
    return tr( "CycleStreets Routing" );
}

QString CycleStreetsPlugin::guiString() const
{
    return tr( "CycleStreets" );
}

QString CycleStreetsPlugin::nameId() const
{
    return QStringLiteral("cyclestreets");
}

QString CycleStreetsPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString CycleStreetsPlugin::description() const
{
    return tr( "Bicycle routing for the United Kingdom using cyclestreets.net" );
}

QString CycleStreetsPlugin::copyrightYears() const
{
    return QStringLiteral("2013");
}

QVector<PluginAuthor> CycleStreetsPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Mihail Ivchenko"), QStringLiteral("ematirov@gmail.com"));
}

RoutingRunner *CycleStreetsPlugin::newRunner() const
{
    return new CycleStreetsRunner;
}

RoutingRunnerPlugin::ConfigWidget *CycleStreetsPlugin::configWidget()
{
    return new CycleStreetsConfigWidget();
}

bool CycleStreetsPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    QSet<RoutingProfilesModel::ProfileTemplate> availableTemplates;
    availableTemplates.insert( RoutingProfilesModel::BicycleTemplate );
    return availableTemplates.contains( profileTemplate );
}

}

#include "CycleStreetsPlugin.moc" // needed for Q_OBJECT here in source
