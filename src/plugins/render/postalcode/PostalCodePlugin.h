//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

#ifndef POSTALCODEPLUGIN_H
#define POSTALCODEPLUGIN_H

#include "AbstractDataPlugin.h"

namespace Marble {

class PostalCodePlugin : public AbstractDataPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.PostalCodePlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( PostalCodePlugin )

 public:
    PostalCodePlugin();

    explicit PostalCodePlugin( const MarbleModel *marbleModel );

    void initialize() override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;
};

}

#endif // POSTALCODEPLUGIN_H
