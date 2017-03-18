//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
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
    Q_PLUGIN_METADATA(IID "org.kde.marble.WlocatePositionProviderPlugin")
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

public:
    WlocatePositionProviderPlugin();
    ~WlocatePositionProviderPlugin() override;

    // Implementing PluginInterface
    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QVector<PluginAuthor> pluginAuthors() const override;
    QIcon icon() const override;
    void initialize() override;
    bool isInitialized() const override;

    // Implementing PositionProviderPlugin
    PositionProviderPlugin * newInstance() const override;

    // Implementing PositionProviderPluginInterface
    PositionProviderStatus status() const override;
    GeoDataCoordinates position() const override;
    qreal speed() const override;
    qreal direction() const override;
    GeoDataAccuracy accuracy() const override;
    QDateTime timestamp() const override;

private Q_SLOTS:
    void update();

    void handleWlocateResult();

private:
    WlocatePositionProviderPluginPrivate* const d;

};

}

#endif // WLOCATE_POSITION_PROVIDER_PLUGIN_H
