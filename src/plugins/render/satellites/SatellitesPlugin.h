//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESPLUGIN_H
#define MARBLE_SATELLITESPLUGIN_H

#include <QtCore/QObject>

#include "AbstractDataPlugin.h"
#include "sgp4/sgp4unit.h"

namespace Marble
{

class PluginAboutDialog;

/**
 * @brief This plugin displays satellites and their orbits.
 *
 */
class SatellitesPlugin : public AbstractDataPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( SatellitesPlugin )

public:
    SatellitesPlugin();
    
    QStringList backendTypes() const;
    QString renderPolicy() const;
    QStringList renderPosition() const;
    QString name() const;
    QString guiString() const;
    QString nameId() const;
    QString description() const;
    QIcon icon() const;

    QDialog *aboutDialog();

    void initialize();
    bool isInitialized() const;

private:
    bool m_isInitialized;
    PluginAboutDialog *m_aboutDialog;
};

}

#endif // MARBLE_SATELLITESPLUGIN_H
