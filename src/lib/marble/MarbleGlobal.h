//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2009 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_GLOBAL_H
#define MARBLE_GLOBAL_H


#include <math.h>

#include <QString>
#include <QColor>

#include "marble_export.h"
#include "MarbleColors.h"

// #define QT_STRICT_ITERATORS


namespace Marble
{

enum TessellationFlag {
    NoTessellation = 0x0,
    Tessellate = 0x1, 
    RespectLatitudeCircle = 0x2,
    FollowGround = 0x4,
    RotationIndicatesFill = 0x8,
    SkipLatLonNormalization = 0x10
};

Q_DECLARE_FLAGS(TessellationFlags, TessellationFlag)

/**
 * @brief This enum is used to choose the projection shown in the view.
 */
enum Projection { 
    Spherical,          ///< Spherical projection
    Equirectangular,    ///< Flat projection ("plate carree")
    Mercator            ///< Mercator projection
};

/**
 * @brief This enum is used to choose the dimension.
 */
enum Dimension {
    Latitude,             ///< Latitude
    Longitude            ///< Longitude
};

Q_DECLARE_FLAGS(Dimensions, Dimension)

/**
 * @brief This enum is used to choose the unit chosen to measure angles.
 */
enum AngleUnit {
    DMSDegree,          ///< Degrees in DMS notation
    DecimalDegree,      ///< Degrees in decimal notation
    UTM                 ///< UTM
};

/**
 * @brief This enum is used to choose context in which map quality gets used.
 */
enum ViewContext {
    Still,              ///< still image
    Animation           ///< animated view (e.g. while rotating the globe)
}; 

/**
 * @brief This enum is used to choose the map quality shown in the view.
 */
enum MapQuality {
    OutlineQuality,     ///< Only a wire representation is drawn
    LowQuality,         ///< Low resolution (e.g. interlaced)
    NormalQuality,      ///< Normal quality
    HighQuality,        ///< High quality (e.g. antialiasing for lines)
    PrintQuality        ///< Print quality
}; 

/**
 * @brief This enum is used to choose which graphics system Qt is using.
 */
enum GraphicsSystem {
    NativeGraphics,     ///< Uses the native graphics system of the OS
    RasterGraphics,     ///< Renders everything onto a pixmap
    OpenGLGraphics      ///< Uses OpenGL
};

/**
 * @brief This enum is used to specify the proxy that is used.
 */
enum ProxyType {
    HttpProxy,          ///< Uses an Http proxy
    Socks5Proxy         ///< Uses a Socks5Proxy
};

/**
 * @brief This enum is used to choose the localization of the labels.
 */
enum LabelPositionFlag {
    NoLabel = 0x0,
    LineStart = 0x1,
    LineCenter = 0x2,
    LineEnd = 0x4,
    IgnoreXMargin = 0x8,
    IgnoreYMargin = 0x10
};

Q_DECLARE_FLAGS(LabelPositionFlags, LabelPositionFlag)

/**
 * @brief This enum is used to choose the localization of the labels.
 */
enum LabelLocalization {
    CustomAndNative,    ///< Custom and native labels
    Custom,             ///< Shows the name in the user's language
    Native              ///< Display the name in the official language and  
                        ///  glyphs of the labeled place. 
};

/**
 * @brief This enum is used to choose how the globe behaves while dragging.
 */
enum DragLocation {
    KeepAxisVertically, ///< Keep planet axis vertically
    FollowMousePointer  ///< Follow mouse pointer exactly
};

/**
 * @brief This enum is used to choose how the globe behaves while dragging.
 */
enum OnStartup {
    ShowHomeLocation,   ///< Show home location on startup
    LastLocationVisited ///< Show last location visited on quit
};

enum AltitudeMode {
    ClampToGround,      ///< Altitude always sticks to ground level
    RelativeToGround,   ///< Altitude is always given relative to ground level
    Absolute            ///< Altitude is given relative to the sealevel
};

enum Pole {
    AnyPole,             ///< Any pole
    NorthPole,           ///< Only North Pole
    SouthPole            ///< Only South Pole
};

/**
 * @brief This enum is used to describe the type of download
 */
enum DownloadUsage {
    DownloadBulk,       ///< Bulk download, for example "File/Download region"
    DownloadBrowse      ///< Browsing mode, normal operation of Marble, like a web browser
};

/** 
 * @brief Describes possible flight mode (interpolation between source
 *        and target camera positions)
 */
enum FlyToMode {
    Automatic, ///< A sane value is chosen automatically depending on animation settings and the action
    Instant, ///< Change camera position immediately (no interpolation)
    Linear, ///< Linear interpolation of lon, lat and distance to ground
    Jump ///< Linear interpolation of lon and lat, distance increases towards the middle point, then decreases    
};

/**
 * @brief Search mode: Global (worldwide) versus area (local, regional) search
 */
enum SearchMode {
    GlobalSearch, ///< Search a whole planet
    AreaSearch ///< Search a certain region of a planet (e.g. visible region)
};

const int defaultLevelZeroColumns = 2;
const int defaultLevelZeroRows = 1;

// Conversion Metric / Imperial System: km vs. miles
const qreal MI2KM = 1.609344;
const qreal KM2MI = 1.0 / MI2KM;

// Conversion Metric / Imperial System: meter vs. feet
const qreal M2FT = 3.2808;
const qreal FT2M = 1.0 / M2FT;

// Conversion meter vs millimeter
const qreal M2MM = 1000;
const qreal MM2M = 1.0 / M2MM;

// Conversion degree vs. radians
const qreal DEG2RAD = M_PI / 180.0;
const qreal RAD2DEG = 180.0 / M_PI;

// Conversion meter vs kilometer
const qreal KM2METER = 1000.0;
const qreal METER2KM = 1.0 / KM2METER;

//Conversion hour vs minute
const qreal HOUR2MIN = 60.0;
const qreal MIN2HOUR = 1.0 / HOUR2MIN;

//Conversion (time) minute vs second
const qreal MIN2SEC = 60.0;
const qreal SEC2MIN = 1.0 / MIN2SEC;

//Conversion hour vs second
const qreal HOUR2SEC = 3600.0;
const qreal SEC2HOUR = 1.0 / HOUR2SEC;

// Version definitions to use with an external application (as digiKam)

// String for about dialog and http user agent
// FIXME: check if blanks are allowed in user agent version numbers
const QString MARBLE_VERSION_STRING = QString::fromLatin1( "0.16.85 (0.17 Beta2)" );

// API Version id:
// form : 0xMMmmpp
//        MM = major revision.
//        mm = minor revision.
//        pp = patch revision.
#define MARBLE_VERSION 0x001055

static const char NOT_AVAILABLE[] = QT_TR_NOOP("not available");

const int tileDigits = 6;

// Average earth radius in m
// Deprecated: Please use model()->planetRadius() instead.
const qreal EARTH_RADIUS = 6378000.0;

// Maximum level of base tiles
const int maxBaseTileLevel = 4;

// Default size (width and height) of tiles
const unsigned int c_defaultTileSize = 675;

class MarbleGlobalPrivate;
class MarbleLocale;

#ifdef __GNUC__
#define MARBLE_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define MARBLE_DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement MARBLE_DEPRECATED for this compiler in MarbleGlobal.h")
#define MARBLE_DEPRECATED(func) func
#endif

class  MARBLE_EXPORT MarbleGlobal
{
 public:
    static MarbleGlobal * getInstance();
    ~MarbleGlobal();

    MarbleLocale * locale() const;
    
    enum Profile {
        Default = 0x0,
        SmallScreen = 0x1,
        HighResolution = 0x2
    };
    
    Q_DECLARE_FLAGS( Profiles, Profile )

    Profiles profiles() const;
    void setProfiles( Profiles profiles );
    
    /**
     * Automatically detects the profile.
     */
    static Profiles detectProfiles();
    
 private:
    MarbleGlobal();

    Q_DISABLE_COPY( MarbleGlobal )
    MarbleGlobalPrivate  * const d;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS( Marble::TessellationFlags )
Q_DECLARE_OPERATORS_FOR_FLAGS( Marble::LabelPositionFlags )
Q_DECLARE_OPERATORS_FOR_FLAGS( Marble::MarbleGlobal::Profiles )

#endif
