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
#include <global.h>
%End


namespace Marble
{

enum TessellationFlag
{
    NoTessellation,
    Tessellate,
    RespectLatitudeCircle,
    FollowGround,
    RotationIndicatesFill
};

typedef QFlags<Marble::TessellationFlag> TessellationFlags;


enum Projection
{
    Spherical,
    Equirectangular,
    Mercator
};


enum DistanceUnit
{
    Metric,
    Imperial
};


enum Dimension
{
    Latitude,
    Longitude
};

typedef QFlags<Marble::Dimension> Dimensions;


enum AngleUnit
{
    DMSDegree,
    DecimalDegree,
    UTM
};


enum ViewContext
{
    Still,
    Animation
};


enum MapQuality
{
    Outline,
    Low,
    Normal,
    High,
    Print
};


enum GraphicsSystem
{
    NativeGraphics,
    RasterGraphics,
    OpenGLGraphics
};


enum LabelPositionFlag
{
    NoLabel,
    LineStart,
    LineCenter,
    LineEnd,
    IgnoreXMargin,
    IgnoreYMargin
};

typedef QFlags<Marble::LabelPositionFlag> LabelPositionFlags;


enum LabelLocalization
{
    CustomAndNative,
    Custom,
    Native
};


enum DragLocation
{
    KeepAxisVertically,
    FollowMousePointer
};


enum OnStartup
{
    ShowHomeLocation,
    LastLocationVisited
};


enum AltitudeMode
{
    ClampToGround,
    RelativeToGround,
    Absolute
};


enum Pole
{
    AnyPole,
    NorthPole,
    SouthPole
};


class MarbleGlobal
{
%TypeHeaderCode
#include <marble/global.h>
%End

public:
    static Marble::MarbleGlobal*  getInstance ();


//mt undefined symbol: MarbleLocale* None -- need mapped type?
//mt     MarbleLocale*      locale () const;



private:
                            MarbleGlobal ();
                            MarbleGlobal (const Marble::MarbleGlobal&);
};   // MarbleGlobal

};   // Marble


