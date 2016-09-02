//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Utku Aydın <utkuaydin34@gmail.com>
//

#include "FoursquarePlugin.h"
#include "FoursquareModel.h"

#include <QSettings>
#include <QIcon>

namespace Marble {

FoursquarePlugin::FoursquarePlugin()
    : AbstractDataPlugin( 0 )
{
}

FoursquarePlugin::FoursquarePlugin(const MarbleModel* marbleModel)
    : AbstractDataPlugin(marbleModel)
{
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
}

void FoursquarePlugin::initialize()
{
    FoursquareModel *model = new FoursquareModel( marbleModel(), this );
    setModel( model );
    setNumberOfItems( 20 ); // Do we hardcode that?
}

QString FoursquarePlugin::name() const
{
    return tr( "Places" );
}

QString FoursquarePlugin::guiString() const
{
    return tr( "&Places" ); // TODO: Check if that ampersand conflicts with another
}

QString FoursquarePlugin::nameId() const
{
    return QStringLiteral("foursquare");
}

QString FoursquarePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString FoursquarePlugin::description() const
{
    return tr( "Displays trending Foursquare places" );
}

QString FoursquarePlugin::copyrightYears() const
{
    return QStringLiteral("2012");
}

QVector<PluginAuthor> FoursquarePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
            << PluginAuthor(QStringLiteral("Utku Aydın"), QStringLiteral("utkuaydin34@gmail.com"));
}

QIcon FoursquarePlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/places.png"));
}

bool FoursquarePlugin::isAuthenticated()
{
    QSettings settings;

    return !settings.value(QStringLiteral("access_token")).isNull();
}

bool FoursquarePlugin::storeAccessToken(const QString& tokenUrl)
{
    QString expected = "http://edu.kde.org/marble/dummy#access_token=";
    if( tokenUrl.startsWith( expected ) ) {
        QSettings settings;
        QString url = tokenUrl;
        settings.setValue(QStringLiteral("access_token"), url.remove(expected));
        return true;
    } else {
        return false;
    }
}

}

#include "moc_FoursquarePlugin.cpp"

