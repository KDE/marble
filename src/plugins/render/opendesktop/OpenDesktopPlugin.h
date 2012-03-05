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

namespace Ui {
    class OpenDesktopConfigWidget;
}
 
namespace Marble {

const int defaultItemsOnScreen = 15;
 
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
    
    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon() const;

    QDialog *configDialog();

    QHash<QString,QVariant> settings() const;

    void setSettings( QHash<QString,QVariant> settings );

private Q_SLOTS:
    void readSettings();

    void writeSettings();
 
 private:
    bool m_isInitialized;

    int m_itemsOnScreen;

    QHash<QString,QVariant> m_settings;
    QDialog * m_configDialog;
    Ui::OpenDesktopConfigWidget * m_uiConfigWidget;
};
 
}
 
#endif // OPENDESKTOPPLUGIN_H
