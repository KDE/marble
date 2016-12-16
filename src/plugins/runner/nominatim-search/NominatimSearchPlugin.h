//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#ifndef MARBLE_NOMINATIMSEARCHPLUGIN_H
#define MARBLE_NOMINATIMSEARCHPLUGIN_H

#include "SearchRunnerPlugin.h"

namespace Marble
{

class NominatimPlugin : public SearchRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.NominatimSearchPlugin")
    Q_INTERFACES( Marble::SearchRunnerPlugin )

public:
    explicit NominatimPlugin( QObject *parent = 0 );

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    SearchRunner* newRunner() const override;
};

}

#endif
