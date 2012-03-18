//
// This file is part of the Marble Virtual Globe.
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
#include "DialogConfigurationInterface.h"
#include "RenderPlugin.h"
#include "RenderPluginInterface.h"

#include <QtCore/QHash>
#include <QtGui/QIcon>

namespace Ui
{
    class WikipediaConfigWidget;
}

namespace Marble
{

class WikipediaPlugin : public AbstractDataPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( WikipediaPlugin )
    
 public:
    WikipediaPlugin();
    ~WikipediaPlugin();
     
    void initialize();

    bool isInitialized() const;

    QString name() const;
    
    QString guiString() const;
    
    QString version() const;

    QString copyrightYears() const;

    QString description() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon() const;

    QDialog *configDialog();

    /**
     * @return: The settings of the item.
     */
    virtual QHash<QString,QVariant> settings() const;

    /**
     * Set the settings of the item.
     */
    virtual void setSettings( const QHash<QString,QVariant> &settings );

 private Q_SLOTS:
    void readSettings();
    void writeSettings();

    void updateSettings();
    void checkNumberOfItems( quint32 number );

 private:
    bool m_isInitialized;

    QIcon m_icon;
    Ui::WikipediaConfigWidget *ui_configWidget;
    QDialog *m_configDialog;
    QHash<QString,QVariant> m_settings;
};

}

#endif // WIKIPEDIAPLUGIN_H
