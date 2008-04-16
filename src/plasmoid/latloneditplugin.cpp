/*
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of
** the License or (at your option) version 3 or any later version
** accepted by the membership of KDE e.V. (or its successor approved
** by the membership of KDE e.V.), which shall act as a proxy 
** defined in Section 14 of version 3 of the license.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//(C) 2008 Henry de Valence

#include "latloneditplugin.h"

#include <QObject>
#include <QDesignerCustomWidgetInterface>
#include <QString>
#include <QWidget>
#include <QIcon>

#include "latlonedit.h"

LatLonEditPlugin::LatLonEditPlugin(QObject *parent = 0)
{
	m_initialized = false;
}
bool LatLonEditPlugin::isInitialized() const
{
	return m_initialized;
void LatLonEditPlugin::initialize(QDesignerFormEditorInterface *core)
{
	m_initialized = true;
}
bool LatLonEditPlugin::isContainer() const
{
	return false;
}
QIcon LatLonEditPlugin::icon() const
{
	return QIcon();
}
QString LatLonEditPlugin::domXml() const
{
	return "<widget class=\"LatLonEdit\" name=\"LatLonEdit\">\n"
	       "</widget>\n";
}
QString LatLonEditPlugin::group() const
{
	return QString( tr( "Input Widgets" ) );
}
QString LatLonEditPlugin::includeFile() const
{
	return QString( "latlonedit.h" );
}
QString LatLonEditPlugin::name() const
{
	return QString( "LatLonEdit" );
}
QString LatLonEditPlugin::toolTip() const
{
	return QString( "" );
}
QString LatLonEditPlugin::whatsThis() const
{
	return QString( "" );
}
QWidget *LatLonEditPlugin::createWidget(QWidget *parent)
{
	return new LatLonEdit(parent);
}

Q_EXPORT_PLUGIN2(latLonEdit, LatLonEdit)
