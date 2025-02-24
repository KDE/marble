// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
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
    ~CycleStreetsConfigWidget() override;
    void loadSettings(const QHash<QString, QVariant> &settings) override;
    QHash<QString, QVariant> settings() const override;

private:
    Ui::CycleStreetsConfigWidget *const ui_configWidget;
};

CycleStreetsConfigWidget::CycleStreetsConfigWidget()
    : RoutingRunnerPlugin::ConfigWidget()
    , ui_configWidget(new Ui::CycleStreetsConfigWidget)
{
    ui_configWidget->setupUi(this);

    ui_configWidget->plan->addItem(tr("balanced"), QStringLiteral("balanced"));
    ui_configWidget->plan->addItem(tr("fastest"), QStringLiteral("fastest"));
    ui_configWidget->plan->addItem(tr("quietest"), QStringLiteral("quietest"));

    if (MarbleGlobal::getInstance()->locale()->measurementSystem() == MarbleLocale::MetricSystem) {
        ui_configWidget->speed->addItem(tr("slow (16 km/h)"), QStringLiteral("16"));
        ui_configWidget->speed->addItem(tr("normal (20 km/h)"), QStringLiteral("20"));
        ui_configWidget->speed->addItem(tr("fast (24 km/h)"), QStringLiteral("24"));
    } else {
        ui_configWidget->speed->addItem(tr("slow (10 mph)"), QStringLiteral("16"));
        ui_configWidget->speed->addItem(tr("normal (12 mph)"), QStringLiteral("20"));
        ui_configWidget->speed->addItem(tr("fast (15 mph)"), QStringLiteral("24"));
    }
}

CycleStreetsConfigWidget::~CycleStreetsConfigWidget()
{
    delete ui_configWidget;
}

void CycleStreetsConfigWidget::loadSettings(const QHash<QString, QVariant> &settings_)
{
    QHash<QString, QVariant> settings = settings_;

    // Check if all fields are filled and fill them with default values.
    if (!settings.contains(QStringLiteral("plan"))) {
        settings.insert(QStringLiteral("plan"), QStringLiteral("balanced"));
    }
    if (!settings.contains(QStringLiteral("speed"))) {
        settings.insert(QStringLiteral("speed"), QStringLiteral("20"));
    }
    ui_configWidget->plan->setCurrentIndex(ui_configWidget->plan->findData(settings.value(QStringLiteral("plan"))));
    ui_configWidget->speed->setCurrentIndex(ui_configWidget->speed->findData(settings.value(QStringLiteral("speed"))));
}

QHash<QString, QVariant> CycleStreetsConfigWidget::settings() const
{
    QHash<QString, QVariant> settings;
    settings.insert(QStringLiteral("plan"), ui_configWidget->plan->itemData(ui_configWidget->plan->currentIndex()));
    settings.insert(QStringLiteral("speed"), ui_configWidget->speed->itemData(ui_configWidget->speed->currentIndex()));
    return settings;
}

CycleStreetsPlugin::CycleStreetsPlugin(QObject *parent)
    : RoutingRunnerPlugin(parent)
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline(false);
    setStatusMessage(tr("This service requires an Internet connection."));
}

QString CycleStreetsPlugin::name() const
{
    return tr("CycleStreets Routing");
}

QString CycleStreetsPlugin::guiString() const
{
    return tr("CycleStreets");
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
    return tr("Bicycle routing for the United Kingdom using cyclestreets.net");
}

QString CycleStreetsPlugin::copyrightYears() const
{
    return QStringLiteral("2013");
}

QList<PluginAuthor> CycleStreetsPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Mihail Ivchenko"), QStringLiteral("ematirov@gmail.com"));
}

RoutingRunner *CycleStreetsPlugin::newRunner() const
{
    return new CycleStreetsRunner;
}

RoutingRunnerPlugin::ConfigWidget *CycleStreetsPlugin::configWidget()
{
    return new CycleStreetsConfigWidget();
}

bool CycleStreetsPlugin::supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const
{
    return profileTemplate == RoutingProfilesModel::BicycleTemplate;
}

}

#include "CycleStreetsPlugin.moc" // needed for Q_OBJECT here in source

#include "moc_CycleStreetsPlugin.cpp"
