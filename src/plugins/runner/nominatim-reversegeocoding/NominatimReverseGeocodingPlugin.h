//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#ifndef MARBLE_NOMINATIMREVERSEGEOCODINGPLUGIN_H
#define MARBLE_NOMINATIMREVERSEGEOCODINGPLUGIN_H

#include "ReverseGeocodingRunnerPlugin.h"

namespace Marble
{

class NominatimPlugin : public ReverseGeocodingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.NominatimReverseGeocodingPlugin")
    Q_INTERFACES( Marble::ReverseGeocodingRunnerPlugin )

public:
    explicit NominatimPlugin( QObject *parent = nullptr );

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    ReverseGeocodingRunner* newRunner() const override;
};

}

#endif
