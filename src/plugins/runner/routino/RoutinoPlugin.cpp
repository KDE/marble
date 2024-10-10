// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "RoutinoPlugin.h"
#include "MarbleDirs.h"
#include "RoutinoRunner.h"

#include "ui_RoutinoConfigWidget.h"

namespace Marble
{

RoutinoPlugin::RoutinoPlugin(QObject *parent)
    : RoutingRunnerPlugin(parent)
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline(true);
}

QString RoutinoPlugin::name() const
{
    return tr("Routino Routing");
}

QString RoutinoPlugin::guiString() const
{
    return tr("Routino");
}

QString RoutinoPlugin::nameId() const
{
    return QStringLiteral("routino");
}

QString RoutinoPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString RoutinoPlugin::description() const
{
    return tr("Retrieves routes from routino");
}

QString RoutinoPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QList<PluginAuthor> RoutinoPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

RoutingRunner *RoutinoPlugin::newRunner() const
{
    return new RoutinoRunner;
}

class RoutinoConfigWidget : public RoutingRunnerPlugin::ConfigWidget
{
    Q_OBJECT
public:
    RoutinoConfigWidget()
        : RoutingRunnerPlugin::ConfigWidget()
    {
        ui_configWidget = new Ui::RoutinoConfigWidget;
        ui_configWidget->setupUi(this);
        QStringList transports;
        // TODO: read from profiles.xml
        ui_configWidget->transport->addItem(tr("Pedestrian"), QStringLiteral("foot"));
        ui_configWidget->transport->addItem(tr("Horse"), QStringLiteral("horse"));
        ui_configWidget->transport->addItem(tr("Wheelchair"), QStringLiteral("wheelchair"));
        ui_configWidget->transport->addItem(tr("Bicycle"), QStringLiteral("bicycle"));
        ui_configWidget->transport->addItem(tr("Moped"), QStringLiteral("moped"));
        ui_configWidget->transport->addItem(tr("Motorbike"), QStringLiteral("motorbike"));
        ui_configWidget->transport->addItem(tr("Motorcar"), QStringLiteral("motorcar"));
        ui_configWidget->transport->addItem(tr("Small lorry"), QStringLiteral("goods"));
        ui_configWidget->transport->addItem(tr("Large lorry"), QStringLiteral("hgv"));
        ui_configWidget->transport->addItem(tr("Public Service Vehicle"), QStringLiteral("psv"));
    }
    ~RoutinoConfigWidget() override
    {
        delete ui_configWidget;
    }
    void loadSettings(const QHash<QString, QVariant> &settings_) override
    {
        QHash<QString, QVariant> settings = settings_;

        // Check if all fields are filled and fill them with default values.
        if (!settings.contains(QStringLiteral("transport"))) {
            settings.insert(QStringLiteral("transport"), QStringLiteral("motorcar"));
        }
        ui_configWidget->transport->setCurrentIndex(ui_configWidget->transport->findData(settings.value(QStringLiteral("transport")).toString()));
        if (settings.value(QStringLiteral("method")).toString() == QLatin1StringView("shortest")) {
            ui_configWidget->shortest->setChecked(true);
        } else {
            ui_configWidget->fastest->setChecked(true);
        }
    }

    QHash<QString, QVariant> settings() const override
    {
        QHash<QString, QVariant> settings;
        settings.insert(QStringLiteral("transport"), ui_configWidget->transport->itemData(ui_configWidget->transport->currentIndex()));

        if (ui_configWidget->shortest->isChecked()) {
            settings.insert(QStringLiteral("method"), QStringLiteral("shortest"));
        } else {
            settings.insert(QStringLiteral("method"), QStringLiteral("fastest"));
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
    return (profileTemplate == RoutingProfilesModel::CarFastestTemplate) || (profileTemplate == RoutingProfilesModel::CarShortestTemplate)
        || (profileTemplate == RoutingProfilesModel::BicycleTemplate) || (profileTemplate == RoutingProfilesModel::PedestrianTemplate);
}

QHash<QString, QVariant> RoutinoPlugin::templateSettings(RoutingProfilesModel::ProfileTemplate profileTemplate) const
{
    QHash<QString, QVariant> result;
    switch (profileTemplate) {
    case RoutingProfilesModel::CarFastestTemplate:
        result.insert(QStringLiteral("transport"), QStringLiteral("motorcar"));
        result.insert(QStringLiteral("method"), QStringLiteral("fastest"));
        break;
    case RoutingProfilesModel::CarShortestTemplate:
        result.insert(QStringLiteral("transport"), QStringLiteral("motorcar"));
        result.insert(QStringLiteral("method"), QStringLiteral("shortest"));
        break;
    case RoutingProfilesModel::CarEcologicalTemplate:
        break;
    case RoutingProfilesModel::BicycleTemplate:
        result.insert(QStringLiteral("transport"), QStringLiteral("bicycle"));
        result.insert(QStringLiteral("method"), QStringLiteral("shortest"));
        break;
    case RoutingProfilesModel::PedestrianTemplate:
        result.insert(QStringLiteral("transport"), QStringLiteral("foot"));
        result.insert(QStringLiteral("method"), QStringLiteral("shortest"));
        break;
    case RoutingProfilesModel::LastTemplate:
        Q_ASSERT(false);
        break;
    }
    return result;
}

bool RoutinoPlugin::canWork() const
{
    QDir mapDir = QDir(MarbleDirs::localPath() + QLatin1StringView("/maps/earth/routino/"));
    return mapDir.exists();
}

}

#include "RoutinoPlugin.moc"

#include "moc_RoutinoPlugin.cpp"
