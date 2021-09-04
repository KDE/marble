//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef PANORAMIOPLUGIN_H
#define PANORAMIOPLUGIN_H

#include "AbstractDataPlugin.h"

namespace Marble
{

class PanoramioPlugin : public AbstractDataPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.PanoramioPlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( PanoramioPlugin )

 public:
    explicit PanoramioPlugin( const MarbleModel *marbleModel = 0 );

    QString nameId() const;

    QString version() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    void initialize();

    QString name() const;

    QString guiString() const;

    QString description() const;

    QIcon icon() const;

 protected:
    bool eventFilter( QObject *object, QEvent *event );
};

}

#endif // PANORAMIOPLUGIN_H
