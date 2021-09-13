// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef FOURSQUAREPLUGIN_H
#define FOURSQUAREPLUGIN_H

#include "AbstractDataPlugin.h"

namespace Marble {

class FoursquarePlugin : public AbstractDataPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.FoursquarePlugin" )
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( FoursquarePlugin )

public:
    FoursquarePlugin();

    explicit FoursquarePlugin( const MarbleModel *marbleModel );

    void initialize() override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    /**
     * @brief Checks if there is an access token stored.
     */
    Q_INVOKABLE bool isAuthenticated();

    /**
     * @brief Stores the access token.
     * @param tokenUrl A dummy URL that has a fragment named access_token
     * @see https://developer.foursquare.com/overview/auth
     */
    Q_INVOKABLE bool storeAccessToken(const QString &tokenUrl);
};

}

#endif // FOURSQUAREPLUGIN_H
