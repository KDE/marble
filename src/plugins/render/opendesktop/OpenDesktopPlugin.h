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
#include "DialogConfigurationInterface.h"

namespace Ui {
    class OpenDesktopConfigWidget;
}
 
namespace Marble {

const int defaultItemsOnScreen = 15;
 
class OpenDesktopPlugin : public AbstractDataPlugin, public DialogConfigurationInterface
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.OpenDesktopPlugin")

    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)

    MARBLE_PLUGIN(OpenDesktopPlugin)
 
 public:
    OpenDesktopPlugin();

    explicit OpenDesktopPlugin( const MarbleModel *marbleModel );

    void initialize() override;

    QString name() const override;
    
    QString guiString() const override;

    QString nameId() const override;
    
    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    QDialog *configDialog() override;

    QHash<QString,QVariant> settings() const override;

    void setSettings( const QHash<QString,QVariant> &settings ) override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private Q_SLOTS:
    void readSettings();

    void writeSettings();
 
 private:
    QDialog * m_configDialog;
    Ui::OpenDesktopConfigWidget * m_uiConfigWidget;
};
 
}
 
#endif // OPENDESKTOPPLUGIN_H
