//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Utku Aydın <utkuaydin34@gmail.com>
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

    virtual void initialize();

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const;

    /**
     * @brief Checks if there is an access token stored.
     */
    Q_INVOKABLE bool isAuthenticated();

    /**
     * @brief Stores the access token.
     * @param url A dummy URL that has a fragment named access_token
     * @see https://developer.foursquare.com/overview/auth
     */
    Q_INVOKABLE bool storeAccessToken(const QString &tokenUrl);
};

}

#endif // FOURSQUAREPLUGIN_H
