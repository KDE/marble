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


%ModuleHeaderCode
//ctscc
#include <ExtDateTime.h>
#include <LatLonEdit.h>
#include <MapThemeManager.h>
#include <MapThemeSortFilterProxyModel.h>
#include <MarbleAboutDialog.h>
#include <MarbleAbstractFloatItem.h>
#include <MarbleControlBox.h>
#include <MarbleMap.h>
#include <MarbleModel.h>
#include <MarbleNavigator.h>
#include <MarbleRenderPlugin.h>
#include <MarbleWidget.h>
#include <MarbleWidgetInputHandler.h>
#include <SunLocator.h>
#include <TileCreator.h>
%End

namespace Marble
{

class ExtDateTime : QObject
{
%TypeHeaderCode
#include <ExtDateTime.h>
%End

public:
                            ExtDateTime ();
    int                     year0 ();
    long                    toJDN ();
    qreal                   dayFraction ();
    void                    setNow ();
    QDateTime               datetime ();
    void                    setDateTime (QDateTime datetime);
    void                    setSpeed (int speed);

signals:
    void                    timeChanged ();

protected:
                            ExtDateTime (const Marble::ExtDateTime&);

public:

//  Subclasses of QObject

%ConvertToSubClassCode

    sipClass = NULL;

    if (dynamic_cast<Marble::ExtDateTime*>(sipCpp))
        sipClass = sipClass_Marble_ExtDateTime;
    else if (dynamic_cast<Marble::LatLonEdit*>(sipCpp))
        sipClass = sipClass_Marble_LatLonEdit;
    else if (dynamic_cast<Marble::MapThemeManager*>(sipCpp))
        sipClass = sipClass_Marble_MapThemeManager;
    else if (dynamic_cast<Marble::MapThemeSortFilterProxyModel*>(sipCpp))
        sipClass = sipClass_Marble_MapThemeSortFilterProxyModel;
    else if (dynamic_cast<Marble::MarbleAboutDialog*>(sipCpp))
        sipClass = sipClass_Marble_MarbleAboutDialog;
    else if (dynamic_cast<Marble::MarbleControlBox*>(sipCpp))
        sipClass = sipClass_Marble_MarbleControlBox;
    else if (dynamic_cast<Marble::MarbleMap*>(sipCpp))
        sipClass = sipClass_Marble_MarbleMap;
    else if (dynamic_cast<Marble::MarbleModel*>(sipCpp))
        sipClass = sipClass_Marble_MarbleModel;
    else if (dynamic_cast<Marble::MarbleNavigator*>(sipCpp))
        sipClass = sipClass_Marble_MarbleNavigator;
    else if (dynamic_cast<Marble::MarbleRenderPlugin*>(sipCpp))
        {
            sipClass = sipClass_Marble_MarbleRenderPlugin;
            if (dynamic_cast<Marble::MarbleAbstractFloatItem*>(sipCpp))
                sipClass = sipClass_Marble_MarbleAbstractFloatItem;
        }
    else if (dynamic_cast<Marble::MarbleWidget*>(sipCpp))
        sipClass = sipClass_Marble_MarbleWidget;
//    else if (dynamic_cast<Marble::MarbleWidgetInputHandler*>(sipCpp))
//        {
//            sipClass = sipClass_Marble_MarbleWidgetInputHandler;
//            if (dynamic_cast<Marble::MarbleWidgetDefaultInputHandler*>(sipCpp))
//                sipClass = sipClass_Marble_MarbleWidgetDefaultInputHandler;
//        }
    else if (dynamic_cast<Marble::SunLocator*>(sipCpp))
        sipClass = sipClass_Marble_SunLocator;
    else if (dynamic_cast<Marble::TileCreator*>(sipCpp))
        sipClass = sipClass_Marble_TileCreator;
%End

};   // ExtDateTime

};   // Marble


