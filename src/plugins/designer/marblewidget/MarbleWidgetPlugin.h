//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2005-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

//
// This class is responsible for offering plugin capabilities.
//

#ifndef MARBLEWIDGETPLUGIN_H
#define MARBLEWIDGETPLUGIN_H

#include <marble_export.h>

// Workaround: moc on osx is unable to find this file, when prefix with QtDesigner/
// moc also doesn't respect Q_OS_* macros, otherwise I could ifdef this.
#include <QDesignerCustomWidgetInterface>

class MarbleWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.MarbleWidgetPlugin")
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    explicit MarbleWidgetPlugin(QObject *parent = nullptr);

    bool isContainer() const override;
    bool isInitialized() const override;
    QIcon icon() const override;
    QString domXml() const override;
    QString name() const override;
    QString group() const override;
    QString includeFile() const override;
    QString toolTip() const override;
    QString whatsThis() const override;
    QWidget *createWidget(QWidget *parent) override;

    void initialize(QDesignerFormEditorInterface *core) override;

private:
    bool m_initialized;
};

#endif
