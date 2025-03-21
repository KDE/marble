// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2005-2007 Torsten Rahn <tackat@kde.org>
//

#include "MarbleNavigatorPlugin.h"

#include <QtPlugin>

#include "MarbleNavigator.h"

MarbleNavigatorPlugin::MarbleNavigatorPlugin(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
    // nothing to do
}

void MarbleNavigatorPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    m_initialized = true;
}

bool MarbleNavigatorPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *MarbleNavigatorPlugin::createWidget(QWidget *parent)
{
    return new Marble::MarbleNavigator(parent);
}

QString MarbleNavigatorPlugin::name() const
{
    return QStringLiteral("Marble::MarbleNavigator");
}

QString MarbleNavigatorPlugin::group() const
{
    return QStringLiteral("Marble Virtual Globe");
}

QIcon MarbleNavigatorPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/marble.png"));
}

QString MarbleNavigatorPlugin::toolTip() const
{
    return {};
}

QString MarbleNavigatorPlugin::whatsThis() const
{
    return {};
}

bool MarbleNavigatorPlugin::isContainer() const
{
    return false;
}

QString MarbleNavigatorPlugin::domXml() const
{
    return QStringLiteral(
        "<widget class=\"Marble::MarbleNavigator\" name=\"MarbleNavigator\">\n"
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
        "</widget>\n");
}

QString MarbleNavigatorPlugin::includeFile() const
{
    return QStringLiteral("MarbleNavigator.h");
}

#include "moc_MarbleNavigatorPlugin.cpp"
