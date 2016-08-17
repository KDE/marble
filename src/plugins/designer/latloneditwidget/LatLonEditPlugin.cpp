/*                                                                                                             
// This file is part of the Marble Virtual Globe.                                                              
//                                                                                                             
// This program is free software licensed under the GNU LGPL. You can                                          
// find a copy of this license in LICENSE.txt in the top directory of                                          
// the source code.                                                                                            
*/

//Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "LatLonEditPlugin.h"

#include "../../lib/marble/LatLonEdit.h"

#include <QString>
#include <QtPlugin>
#include <QWidget>
#include <QIcon>
#include <QDesignerCustomWidgetInterface>

LatLonEditPlugin::LatLonEditPlugin(QObject *parent) :
  QObject(parent),
  m_initialized( false )
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
    return "<widget class=\"Marble::LatLonEdit\" name=\"LatLonEdit\">\n"
           "</widget>\n";
}

QString LatLonEditPlugin::group() const
{
    return "Marble Virtual Globe";
}

QString LatLonEditPlugin::includeFile() const
{
    return QString( "LatLonEdit.h" );
}

QString LatLonEditPlugin::name() const
{
    return QString( "Marble::LatLonEdit" );
}

QString LatLonEditPlugin::toolTip() const
{
    return QString();
}

QString LatLonEditPlugin::whatsThis() const
{
    return QString();
}

QWidget *LatLonEditPlugin::createWidget(QWidget *parent)
{
    return new Marble::LatLonEdit(parent);
}

#include "moc_LatLonEditPlugin.cpp"
