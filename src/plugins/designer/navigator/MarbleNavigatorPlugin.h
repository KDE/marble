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

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString name() const;
    QString group() const;
    QString includeFile() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);

    void initialize(QDesignerFormEditorInterface *core);

private:
    bool m_initialized;
};

#endif
