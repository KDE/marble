//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
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
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
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
    return QStringLiteral("routino");
}

QString RoutinoPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString RoutinoPlugin::description() const
{
    return tr( "Retrieves routes from routino" );
}

QString RoutinoPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QVector<PluginAuthor> RoutinoPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
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
        ui_configWidget->setupUi( this );
        QStringList transports;
        //TODO: read from profiles.xml
        ui_configWidget->transport->addItem(tr("Foot"), "foot");
        ui_configWidget->transport->addItem(tr("Horse"), "horse");
        ui_configWidget->transport->addItem(tr("Wheelchair"), "wheelchair");
        ui_configWidget->transport->addItem(tr("Bicycle"), "bicycle");
        ui_configWidget->transport->addItem(tr("Moped"), "moped");
        ui_configWidget->transport->addItem(tr("Motorbike"), "motorbike");
        ui_configWidget->transport->addItem(tr("Motorcar"), "motorcar");
        ui_configWidget->transport->addItem(tr("Small lorry"), "goods");
        ui_configWidget->transport->addItem(tr("Large lorry"), "hgv");
        ui_configWidget->transport->addItem(tr("Public Service Vehicle"), "psv");
    }
    virtual ~RoutinoConfigWidget()
    {
        delete ui_configWidget;
    }
    virtual void loadSettings( const QHash<QString, QVariant> &settings_ )
    {
        QHash<QString, QVariant> settings = settings_;

        // Check if all fields are filled and fill them with default values.
        if (!settings.contains(QStringLiteral("transport"))) {
            settings.insert(QStringLiteral("transport"), QStringLiteral("motorcar"));
        }
        ui_configWidget->transport->setCurrentIndex(
            ui_configWidget->transport->findData(settings.value(QStringLiteral("transport")).toString()));
        if (settings.value(QStringLiteral("method")).toString() == QLatin1String("shortest")) {
            ui_configWidget->shortest->setChecked( true );
        } else {
            ui_configWidget->fastest->setChecked( true );
        }
    }

    virtual QHash<QString, QVariant> settings() const
    {
        QHash<QString,QVariant> settings;
        settings.insert(QStringLiteral("transport"),
                        ui_configWidget->transport->itemData( ui_configWidget->transport->currentIndex() ) );

        if ( ui_configWidget->shortest->isChecked() ) {
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
            Q_ASSERT( false );
            break;
    }
    return result;
}

bool RoutinoPlugin::canWork() const
{
    QDir mapDir = QDir(MarbleDirs::localPath() + QLatin1String("/maps/earth/routino/"));
    return mapDir.exists();
}

}

#include "RoutinoPlugin.moc"
