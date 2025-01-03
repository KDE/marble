// SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>
//  SPDX-License-Identifier: LGPL-2.1-or-later

#include "LatLonEditPlugin.h"

#include "../../lib/marble/LatLonEdit.h"

#include <QIcon>
#include <QString>
#include <QWidget>
#include <QtPlugin>

LatLonEditPlugin::LatLonEditPlugin(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
{
    // nothing to do
}

bool LatLonEditPlugin::isInitialized() const
{
    return m_initialized;
}

void LatLonEditPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core)
    m_initialized = true;
}

bool LatLonEditPlugin::isContainer() const
{
    return false;
}

QIcon LatLonEditPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/marble.png"));
}

QString LatLonEditPlugin::domXml() const
{
    return QStringLiteral(
        "<widget class=\"Marble::LatLonEdit\" name=\"LatLonEdit\">\n"
        "</widget>\n");
}

QString LatLonEditPlugin::group() const
{
    return QStringLiteral("Marble Virtual Globe");
}

QString LatLonEditPlugin::includeFile() const
{
    return QStringLiteral("LatLonEdit.h");
}

QString LatLonEditPlugin::name() const
{
    return QStringLiteral("Marble::LatLonEdit");
}

QString LatLonEditPlugin::toolTip() const
{
    return {};
}

QString LatLonEditPlugin::whatsThis() const
{
    return {};
}

QWidget *LatLonEditPlugin::createWidget(QWidget *parent)
{
    return new Marble::LatLonEdit(parent);
}

#include "moc_LatLonEditPlugin.cpp"
