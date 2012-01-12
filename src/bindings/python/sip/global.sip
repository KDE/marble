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
    RotationIndicatesFill,
    SkipLatLonNormalization
};

typedef QFlags<Marble::TessellationFlag> TessellationFlags;


enum Projection
{
    Spherical,
    Equirectangular,
    Mercator
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
    OutlineQuality,
    LowQuality,
    NormalQuality,
    HighQuality,
    PrintQuality
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
#include <global.h>
%End

public:
    static Marble::MarbleGlobal*  getInstance ();

private:
                            MarbleGlobal ();
public:
    ~MarbleGlobal ();
//ig    MarbleLocale*           locale () const;
    enum Profile
    {
        Default,
        SmallScreen,
        HighResolution
    };
    typedef QFlags<Marble::MarbleGlobal::Profile> Profiles;
    Marble::MarbleGlobal::Profiles  profiles () const;
    void                    setProfiles (Marble::MarbleGlobal::Profiles profiles);
    static Marble::MarbleGlobal::Profiles  detectProfiles ();
};
// MarbleGlobal

enum ProxyType
{
    HttpProxy,
    Socks5Proxy
};
enum DownloadUsage
{
    DownloadBulk,
    DownloadBrowse
};
enum FlyToMode
{
    Automatic,
    Instant,
    Linear,
    Jump
};
};
// Marble


