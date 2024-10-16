// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "OpenRouteServicePlugin.h"
#include "OpenRouteServiceRunner.h"

#include "ui_OpenRouteServiceConfigWidget.h"

namespace Marble
{

OpenRouteServicePlugin::OpenRouteServicePlugin(QObject *parent)
    : RoutingRunnerPlugin(parent)
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline(false);
    setStatusMessage(tr("This service requires an Internet connection."));
}

QString OpenRouteServicePlugin::name() const
{
    return tr("OpenRouteService Routing");
}

QString OpenRouteServicePlugin::guiString() const
{
    return tr("OpenRouteService");
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
    return tr("Routing in Europe using openrouteservice.org");
}

QString OpenRouteServicePlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QList<PluginAuthor> OpenRouteServicePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
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
        ui_configWidget->setupUi(this);

        ui_configWidget->preference->addItem(tr("Car (recommended way)"), QStringLiteral("Recommended"));
        ui_configWidget->preference->addItem(tr("Car (fastest way)"), QStringLiteral("Fastest"));
        ui_configWidget->preference->addItem(tr("Car (shortest way)"), QStringLiteral("Shortest"));
        ui_configWidget->preference->addItem(tr("Pedestrian (shortest way)"), QStringLiteral("Pedestrian"));
        ui_configWidget->preference->addItem(tr("Bicycle (shortest track)"), QStringLiteral("Bicycle"));
        ui_configWidget->preference->addItem(tr("Bicycle (Mountainbike)"), QStringLiteral("BicycleMTB"));
        ui_configWidget->preference->addItem(tr("Bicycle (Racer)"), QStringLiteral("BicycleRacer"));
        ui_configWidget->preference->addItem(tr("Bicycle (safest track)"), QStringLiteral("BicycleSafety"));
        ui_configWidget->preference->addItem(tr("Bicycle (preferred Cycleway/-route)"), QStringLiteral("BicycleTour"));
    }

    ~OpenRouteServiceConfigWidget()
    {
        delete ui_configWidget;
    }

    void loadSettings(const QHash<QString, QVariant> &settings_) override
    {
        QHash<QString, QVariant> settings = settings_;

        // Check if all fields are filled and fill them with default values.
        if (!settings.contains(QStringLiteral("preference"))) {
            settings.insert(QStringLiteral("preference"), QStringLiteral("Fastest"));
        }
        ui_configWidget->preference->setCurrentIndex(ui_configWidget->preference->findData(settings.value(QStringLiteral("preference")).toString()));
        ui_configWidget->noMotorways->setCheckState(static_cast<Qt::CheckState>(settings.value(QStringLiteral("noMotorways")).toInt()));
        ui_configWidget->noTollways->setCheckState(static_cast<Qt::CheckState>(settings.value(QStringLiteral("noTollways")).toInt()));
        ui_configWidget->noFerries->setCheckState(static_cast<Qt::CheckState>(settings.value(QStringLiteral("noFerries")).toInt()));
    }

    QHash<QString, QVariant> settings() const override
    {
        QHash<QString, QVariant> settings;
        settings.insert(QStringLiteral("preference"), ui_configWidget->preference->itemData(ui_configWidget->preference->currentIndex()));
        settings.insert(QStringLiteral("noMotorways"), ui_configWidget->noMotorways->checkState());
        settings.insert(QStringLiteral("noTollways"), ui_configWidget->noTollways->checkState());
        settings.insert(QStringLiteral("noFerries"), ui_configWidget->noFerries->checkState());
        return settings;
    }

private:
    Ui::OpenRouteServiceConfigWidget *ui_configWidget = nullptr;
};

RoutingRunnerPlugin::ConfigWidget *OpenRouteServicePlugin::configWidget()
{
    return new OpenRouteServiceConfigWidget();
}

bool OpenRouteServicePlugin::supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const
{
    return (profileTemplate == RoutingProfilesModel::CarFastestTemplate) || (profileTemplate == RoutingProfilesModel::CarShortestTemplate)
        || (profileTemplate == RoutingProfilesModel::BicycleTemplate) || (profileTemplate == RoutingProfilesModel::PedestrianTemplate);
}

QHash<QString, QVariant> OpenRouteServicePlugin::templateSettings(RoutingProfilesModel::ProfileTemplate profileTemplate) const
{
    QHash<QString, QVariant> result;
    switch (profileTemplate) {
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
        Q_ASSERT(false);
        break;
    }
    return result;
}

}

#include "OpenRouteServicePlugin.moc" // needed for Q_OBJECT here in source

#include "moc_OpenRouteServicePlugin.cpp"
