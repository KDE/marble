//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#ifndef EARTHQUAKEPLUGIN_H
#define EARTHQUAKEPLUGIN_H

#include "AbstractDataPlugin.h"
#include "RenderPlugin.h"
#include "RenderPluginInterface.h"

#include <QtGui/QIcon>

namespace Marble
{

class EarthquakePlugin : public AbstractDataPlugin
{
    Q_OBJECT

    Q_INTERFACES( Marble::RenderPluginInterface )

    MARBLE_PLUGIN( EarthquakePlugin )

public:
    EarthquakePlugin();

    virtual void initialize();

    virtual bool isInitialized() const;

    QString name() const;

    QString guiString() const;

    QString description() const;

    QIcon icon() const;

private:
    bool m_isInitialized;
};

}

#endif // EARTHQUAKEPLUGIN_H
