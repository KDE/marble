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
#include "ui_WikipediaConfigWidget.h"

#include <QtCore/QHash>
#include <QtGui/QIcon>

namespace Marble
{

class PluginAboutDialog;

class WikipediaPlugin : public AbstractDataPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( WikipediaPlugin )
    
 public:
    WikipediaPlugin();
    ~WikipediaPlugin();
     
    void initialize();
    
    QString name() const;
    
    QString guiString() const;
    
    QString description() const;
    
    QIcon icon() const;

    QDialog *aboutDialog() const;

    QDialog *configDialog() const;

    void setShowThumbnails( bool shown );

    /**
     * @return: The settings of the item.
     */
    virtual QHash<QString,QVariant> settings() const;

    /**
     * Set the settings of the item.
     */
    virtual void setSettings( QHash<QString,QVariant> settings );

 private Q_SLOTS:
    void readSettings();
    void writeSettings();

    void setDialogNumberOfItems( quint32 number );

    void updateItemSettings();

 private:
    QIcon m_icon;
    PluginAboutDialog *m_aboutDialog;
    Ui::WikipediaConfigWidget ui_configWidget;
    QDialog *m_configDialog;
    QHash<QString,QVariant> m_settings;
};

}

#endif // WIKIPEDIAPLUGIN_H
