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
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( FoursquarePlugin )

public:
    FoursquarePlugin();
    
    explicit FoursquarePlugin( const MarbleModel *marbleModel );

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

#endif // FOURSQUAREPLUGIN_H
