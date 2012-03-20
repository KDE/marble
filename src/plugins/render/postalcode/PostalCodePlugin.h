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
#include "RenderPlugin.h"
#include "RenderPluginInterface.h"

namespace Marble {

class PostalCodePlugin : public AbstractDataPlugin {
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( PostalCodePlugin )

 public:
    PostalCodePlugin();

    virtual void initialize();

    virtual bool isInitialized() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon() const;

 private:
    bool m_isInitialized;
};

}

#endif // POSTALCODEPLUGIN_H
