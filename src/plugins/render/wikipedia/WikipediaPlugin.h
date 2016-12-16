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

#include <QHash>
#include <QIcon>

namespace Ui
{
    class WikipediaConfigWidget;
}

namespace Marble
{

class WikipediaPlugin : public AbstractDataPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.WikipediaPlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( WikipediaPlugin )
    
 public:
    WikipediaPlugin();
    explicit WikipediaPlugin( const MarbleModel *marbleModel );
    ~WikipediaPlugin() override;
     
    void initialize() override;

    QString name() const override;
    
    QString guiString() const override;

    QString nameId() const override;
    
    QString version() const override;

    QString copyrightYears() const override;

    QString description() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString aboutDataText() const override;

    QIcon icon() const override;

    QDialog *configDialog() override;

    /**
     * @return: The settings of the item.
     */
    QHash<QString,QVariant> settings() const override;

    /**
     * Set the settings of the item.
     */
    void setSettings( const QHash<QString,QVariant> &settings ) override;

 protected:
    bool eventFilter( QObject *object, QEvent *event ) override;

 private Q_SLOTS:
    void readSettings();
    void writeSettings();

    void updateSettings();
    void checkNumberOfItems( quint32 number );

 private:
    const QIcon m_icon;
    Ui::WikipediaConfigWidget *ui_configWidget;
    QDialog *m_configDialog;
    bool m_showThumbnails;
};

}

#endif // WIKIPEDIAPLUGIN_H
