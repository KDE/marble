//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
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
