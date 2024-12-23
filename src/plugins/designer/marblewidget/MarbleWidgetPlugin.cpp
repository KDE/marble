// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2005-2007 Torsten Rahn <tackat@kde.org>
//

#include "MarbleWidgetPlugin.h"

#include <QtPlugin>

#include "../../lib/marble/MarbleModel.h"
#include "../../lib/marble/MarbleWidget.h"

MarbleWidgetPlugin::MarbleWidgetPlugin(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
    // nothing to do
}

void MarbleWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    m_initialized = true;
}

bool MarbleWidgetPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *MarbleWidgetPlugin::createWidget(QWidget *parent)
{
    auto marbleWidget = new Marble::MarbleWidget(parent);
    marbleWidget->setMapThemeId(QStringLiteral("earth/srtm/srtm.dgml"));

    return marbleWidget;
}

QString MarbleWidgetPlugin::name() const
{
    return QStringLiteral("Marble::MarbleWidget");
}

QString MarbleWidgetPlugin::group() const
{
    return QStringLiteral("Marble Virtual Globe");
}

QIcon MarbleWidgetPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/marble.png"));
}

QString MarbleWidgetPlugin::toolTip() const
{
    return {};
}

QString MarbleWidgetPlugin::whatsThis() const
{
    return {};
}

bool MarbleWidgetPlugin::isContainer() const
{
    return false;
}

QString MarbleWidgetPlugin::domXml() const
{
    return QStringLiteral(
        "<widget class=\"Marble::MarbleWidget\" name=\"MarbleWidget\">\n"
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
        "</widget>\n");
}

QString MarbleWidgetPlugin::includeFile() const
{
    return QStringLiteral("MarbleWidget.h");
}

#include "moc_MarbleWidgetPlugin.cpp"
