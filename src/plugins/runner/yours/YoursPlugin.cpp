//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2016      Piotr Wójcik <chocimier@tlen.pl>
//

#include "YoursPlugin.h"
#include "YoursRunner.h"

#include "ui_YoursConfigWidget.h"

namespace Marble
{

YoursPlugin::YoursPlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( false );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString YoursPlugin::name() const
{
    return tr( "Yours Routing" );
}

QString YoursPlugin::guiString() const
{
    return tr( "Yours" );
}

QString YoursPlugin::nameId() const
{
    return QStringLiteral("yours");
}

QString YoursPlugin::version() const
{
    return QStringLiteral("1.1");
}

QString YoursPlugin::description() const
{
    return tr( "Worldwide routing using a YOURS server" );
}

QString YoursPlugin::copyrightYears() const
{
    return QStringLiteral("2010, 2016");
}

QVector<PluginAuthor> YoursPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
            << PluginAuthor(QStringLiteral("Piotr Wójcik"), QStringLiteral("chocimier@tlen.pl"));
}

RoutingRunner *YoursPlugin::newRunner() const
{
    return new YoursRunner;
}

class YoursConfigWidget : public RoutingRunnerPlugin::ConfigWidget
{
Q_OBJECT
public:
    YoursConfigWidget()
        : RoutingRunnerPlugin::ConfigWidget()
    {
        ui_configWidget = new Ui::YoursConfigWidget;
        ui_configWidget->setupUi( this );
        //TODO: read from profiles.xml
        ui_configWidget->transport->addItem( tr( "Foot" ), "foot" );
        ui_configWidget->transport->addItem( tr( "Bicycle" ), "bicycle" );
        ui_configWidget->transport->addItem( tr( "Motorcar" ), "motorcar" );
    }
    virtual ~YoursConfigWidget()
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
    Ui::YoursConfigWidget *ui_configWidget;
};

RoutingRunnerPlugin::ConfigWidget *YoursPlugin::configWidget()
{
    return new YoursConfigWidget();
}

bool YoursPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    QSet<RoutingProfilesModel::ProfileTemplate> availableTemplates;
    availableTemplates.insert( RoutingProfilesModel::CarFastestTemplate );
    availableTemplates.insert( RoutingProfilesModel::CarShortestTemplate );
    availableTemplates.insert( RoutingProfilesModel::BicycleTemplate );
    availableTemplates.insert( RoutingProfilesModel::PedestrianTemplate );
    return availableTemplates.contains( profileTemplate );
}

QHash< QString, QVariant > YoursPlugin::templateSettings(RoutingProfilesModel::ProfileTemplate profileTemplate) const
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

}

#include "YoursPlugin.moc"
