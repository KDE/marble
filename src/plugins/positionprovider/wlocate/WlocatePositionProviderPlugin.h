//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef WLOCATE_POSITION_PROVIDER_PLUGIN_H
#define WLOCATE_POSITION_PROVIDER_PLUGIN_H

#include "PositionProviderPlugin.h"

namespace Marble
{

class WlocatePositionProviderPluginPrivate;

class WlocatePositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

public:
    WlocatePositionProviderPlugin();
    virtual ~WlocatePositionProviderPlugin();

    // Implementing PluginInterface
    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString version() const;
    virtual QString description() const;
    virtual QString copyrightYears() const;
    virtual QList<PluginAuthor> pluginAuthors() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;

    // Implementing PositionProviderPlugin
    virtual PositionProviderPlugin * newInstance() const;

    // Implementing PositionProviderPluginInterface
    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual qreal speed() const;
    virtual qreal direction() const;
    virtual GeoDataAccuracy accuracy() const;
    virtual QDateTime timestamp() const;

private Q_SLOTS:
    void update();

    void handleWlocateResult();

private:
    WlocatePositionProviderPluginPrivate* const d;

};

}

#endif // WLOCATE_POSITION_PROVIDER_PLUGIN_H
