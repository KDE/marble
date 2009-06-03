//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef WIKIPEDIAPLUGIN_H
#define WIKIPEDIAPLUGIN_H

#include "AbstractDataPlugin.h"
#include "RenderPlugin.h"
#include "RenderPluginInterface.h"

#include <QtGui/QIcon>

namespace Marble {

class WikipediaPlugin : public AbstractDataPlugin {
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( WikipediaPlugin )
    
 public:
    WikipediaPlugin();
     
    void initialize();
    
    QString name() const;
    
    QString guiString() const;
    
    QString description() const;
    
    QIcon icon() const;
};

}

#endif // WIKIPEDIAPLUGIN_H
