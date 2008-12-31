//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

%Module PyKDE4.marble

%ModuleHeaderCode
#pragma GCC visibility push(default)

#include <Quaternion.h>
#include <global.h>
%End

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip
%Import QtNetwork/QtNetworkmod.sip
%Import QtXml/QtXmlmod.sip
%Import QtSvg/QtSvgmod.sip
%Import kdecore/kdecoremod.sip
%Import kdeui/kdeuimod.sip

%Include Quaternion.sip
%Include GeoDataCoordinates.sip
//%Include AbstractLayerInterface.sip
%Include ClipPainter.sip

%Include GeoPainter.sip
%Include LatLonEdit.sip
%Include MapThemeManager.sip
%Include MarbleAbstractFloatItem.sip
%Include MarbleControlBox.sip
%Include MarbleDataFacade.sip
%Include MarbleDirs.sip
%Include MarbleMap.sip
%Include MarbleModel.sip
%Include MarbleNavigator.sip
%Include MarbleRenderPlugin.sip
%Include MarbleRenderPluginInterface.sip
%Include MarbleWidget.sip

%Include AbstractProjection.sip
//%Include AbstractProjectionHelper.sip
//%Include EquirectProjection.sip
//%Include EquirectProjectionHelper.sip
//%Include MercatorProjection.sip
//%Include MercatorProjectionHelper.sip
//%Include SphericalProjection.sip
//%Include SphericalProjectionHelper.sip
%Include SunLocator.sip
%Include TileCreator.sip
%Include TileCreatorDialog.sip
%Include ViewParams.sip
%Include ViewportParams.sip
%Include GeoDataColorStyle.sip
%Include GeoDataContainer.sip
%Include GeoDataDocument.sip
%Include GeoDataFeature.sip
%Include GeoDataFolder.sip
%Include GeoDataGeometry.sip
%Include GeoDataHotSpot.sip
%Include GeoDataIconStyle.sip
%Include GeoDataLabelStyle.sip
%Include GeoDataLatLonAltBox.sip
%Include GeoDataLatLonBox.sip
%Include GeoDataLineString.sip
%Include GeoDataLineStyle.sip
%Include GeoDataLinearRing.sip
%Include GeoDataMultiGeometry.sip
%Include GeoDataObject.sip
%Include GeoDataPlacemark.sip
%Include GeoDataPoint.sip
%Include GeoDataPolyStyle.sip
%Include GeoDataPolygon.sip
%Include GeoDataStyle.sip
%Include GeoDataStyleMap.sip
%Include GeoDataStyleSelector.sip
%Include Serializable.sip
%Include GeoDocument.sip
%Include global.sip
%Include MarbleAboutDialog.sip

// ---- API isn't exported.
//%Include MapThemeSortFilterProxyModel.sip
//%Include ExtDateTime.sip
//%Include MarbleWidgetInputHandler.sip

// ---- Not exported, and is deprecated
//%Include BoundingBox.sip

