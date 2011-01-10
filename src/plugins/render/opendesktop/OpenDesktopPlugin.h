//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef OPENDESKTOPPLUGIN_H
#define OPENDESKTOPPLUGIN_H

#include "AbstractDataPlugin.h"
#include "RenderPlugin.h"
#include "RenderPluginInterface.h"
#include <QtGui/QIcon>
 
namespace Marble {
 
class OpenDesktopPlugin : public AbstractDataPlugin {

    Q_OBJECT

    Q_INTERFACES(Marble::RenderPluginInterface)

    MARBLE_PLUGIN(OpenDesktopPlugin)
 
 public:
    OpenDesktopPlugin();
    
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
 
#endif // OPENDESKTOPPLUGIN_H
