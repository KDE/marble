//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_ROUTINOPLUGIN_H
#define MARBLE_ROUTINOPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Ui {
    class RoutinoConfigWidget;
}
namespace Marble
{

class RoutinoPlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    explicit RoutinoPlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    virtual MarbleAbstractRunner* newRunner() const;

    ConfigWidget* configWidget();

    bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const;

    QHash< QString, QVariant > templateSettings(RoutingProfilesModel::ProfileTemplate profileTemplate) const;

    virtual bool canWork() const;
};

}

#endif
