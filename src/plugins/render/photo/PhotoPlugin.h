//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef PHOTOPLUGIN_H
#define PHOTOPLUGIN_H

#include "AbstractDataPlugin.h"

class QIcon;

namespace Marble
{

class PhotoPlugin : public AbstractDataPlugin
{
    Q_OBJECT

    Q_INTERFACES( Marble::RenderPluginInterface )

    MARBLE_PLUGIN( PhotoPlugin )
    
 public:
    PhotoPlugin();
     
    void initialize();

    bool isInitialized () const;
    
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

#endif //PHOTOPLUGIN_H
