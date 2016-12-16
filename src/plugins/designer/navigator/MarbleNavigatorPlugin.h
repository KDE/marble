//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
//

//
// This class is responsible for offering plugin capabilities.
//

#ifndef MARBLENAVIGATORPLUGIN_H
#define MARBLENAVIGATORPLUGIN_H

#include <marble_export.h>

// Workaround: moc on osx is unable to find this file, when prefix with QtDesigner/
// moc also doesn't respect Q_OS_* macros, otherwise I could ifdef this.
#include <QDesignerCustomWidgetInterface>

class MarbleNavigatorPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.MarbleNavigatorPlugin")
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    explicit MarbleNavigatorPlugin(QObject *parent = 0);

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
