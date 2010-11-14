//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
//

#include "MarbleNavigatorPlugin.h"

#include <QtCore/QtPlugin>

#include "MarbleNavigator.h"


MarbleNavigatorPlugin::MarbleNavigatorPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

MarbleNavigatorPlugin::~MarbleNavigatorPlugin()
{
}

void MarbleNavigatorPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool MarbleNavigatorPlugin::isInitialized() const
{
    return initialized;
}

QWidget *MarbleNavigatorPlugin::createWidget(QWidget *parent)
{
    // Create the Model (Globe) and one view.
    return new Marble::MarbleNavigator( parent );
}

QString MarbleNavigatorPlugin::name() const
{
    return "Marble::MarbleNavigator";
}

QString MarbleNavigatorPlugin::group() const
{
    return "Marble Desktop Globe";
}

QIcon MarbleNavigatorPlugin::icon() const
{
    return QIcon(":/icons/marble.png");
}

QString MarbleNavigatorPlugin::toolTip() const
{
    return "";
}

QString MarbleNavigatorPlugin::whatsThis() const
{
    return "";
}

bool MarbleNavigatorPlugin::isContainer() const
{
    return false;
}

QString MarbleNavigatorPlugin::domXml() const
{
    return "<widget class=\"Marble::MarbleNavigator\" name=\"MarbleNavigator\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>200</width>\n"
           "   <height>200</height>\n"
           "  </rect>\n"
           " </property>\n"
           " <property name=\"toolTip\" >\n"
           "  <string>Navigation Control</string>\n"
           " </property>\n"
           " <property name=\"whatsThis\" >\n"
           "  <string>Use this control to navigate.</string>\n"
           " </property>\n"
           "</widget>\n";
}

QString MarbleNavigatorPlugin::includeFile() const
{
    return "MarbleNavigator.h";
}

Q_EXPORT_PLUGIN2(MarbleNavigatorPlugin, MarbleNavigatorPlugin)

#include "MarbleNavigatorPlugin.moc"
