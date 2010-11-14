//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
//

#include "MarbleWidgetPlugin.h"

#include <QtCore/QtPlugin>

#include "../../lib/MarbleModel.h"
#include "../../lib/MarbleWidget.h"


MarbleWidgetPlugin::MarbleWidgetPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

MarbleWidgetPlugin::~MarbleWidgetPlugin()
{
}

void MarbleWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool MarbleWidgetPlugin::isInitialized() const
{
    return initialized;
}

QWidget *MarbleWidgetPlugin::createWidget(QWidget *parent)
{
    Marble::MarbleWidget* marbleWidget = new Marble::MarbleWidget( parent );
    marbleWidget->setMapThemeId("earth/srtm/srtm.dgml");

    return marbleWidget;
}

QString MarbleWidgetPlugin::name() const
{
    return "Marble::MarbleWidget";
}

QString MarbleWidgetPlugin::group() const
{
    return "Marble Desktop Globe";
}

QIcon MarbleWidgetPlugin::icon() const
{
    return QIcon(":/icons/marble.png");
}

QString MarbleWidgetPlugin::toolTip() const
{
    return "";
}

QString MarbleWidgetPlugin::whatsThis() const
{
    return "";
}

bool MarbleWidgetPlugin::isContainer() const
{
    return false;
}

QString MarbleWidgetPlugin::domXml() const
{
    return "<widget class=\"Marble::MarbleWidget\" name=\"MarbleWidget\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>200</width>\n"
           "   <height>200</height>\n"
           "  </rect>\n"
           " </property>\n"
           " <property name=\"toolTip\" >\n"
           "  <string>The Earth</string>\n"
           " </property>\n"
           " <property name=\"whatsThis\" >\n"
           "  <string>The Marble widget displays a virtual globe.</string>\n"
           " </property>\n"
           "</widget>\n";
}

QString MarbleWidgetPlugin::includeFile() const
{
    return "MarbleWidget.h";
}

Q_EXPORT_PLUGIN2(MarbleWidgetPlugin, MarbleWidgetPlugin)

#include "MarbleWidgetPlugin.moc"
