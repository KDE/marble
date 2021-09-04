// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007-2009 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

#ifndef MARBLE_GLOBAL_H
#define MARBLE_GLOBAL_H


#include <cmath>

#include <QString>

#include "marble_export.h"
#include "marble_version.h"

// #define QT_STRICT_ITERATORS

/* M_PI is a #define that may or may not be handled in <cmath> */
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419717
#endif

namespace Marble
{

enum TessellationFlag {
    NoTessellation = 0x0,
    Tessellate = 0x1, 
    RespectLatitudeCircle = 0x2,
    FollowGround = 0x4,
    PreventNodeFiltering = 0x8
};

Q_DECLARE_FLAGS(TessellationFlags, TessellationFlag)

/**
 * @brief This enum is used to choose the projection shown in the view.
 */
enum Projection { 
    Spherical,          ///< Spherical projection ("Orthographic")
    Equirectangular,    ///< Flat projection ("plate carree")
    Mercator,           ///< Mercator projection
    Gnomonic,           ///< Gnomonic projection
    Stereographic,      ///< Stereographic projection
    LambertAzimuthal,   ///< Lambert Azimuthal Equal-Area projection
    AzimuthalEquidistant,   ///< Azimuthal Equidistant projection
    VerticalPerspective ///< Vertical perspective projection
    // NOTE: MarbleWidget::setProjection(int) relies on VerticalPerspective being the last
    // value above. Adjust that method if you do changes here
};

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
    IgnoreYMargin = 0x10,
    FollowLine = 0x20
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
    Absolute,            ///< Altitude is given relative to the sealevel
    RelativeToSeaFloor,  ///< Altitude is given relative to the sea floor
    ClampToSeaFloor      ///< Altitude always sticks to sea floor
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

/**
 * @brief
 */
enum RenderStatus {
    Complete, ///< All data is there and up to date
    WaitingForUpdate, ///< Rendering is based on complete, but outdated data, data update was requested
    WaitingForData, ///< Rendering is based on no or partial data, more data was requested (e.g. pending network queries)
    Incomplete ///< Data is missing and some error occurred when trying to retrieve it (e.g. network failure)
};

const int defaultLevelZeroColumns = 2;
const int defaultLevelZeroRows = 1;

// Conversion Metric / Imperial System: km vs. miles
const qreal MI2KM = 1.609344;
const qreal KM2MI = 1.0 / MI2KM;

// Conversion Nautical / Imperial System: nm vs. km
const qreal NM2KM = 1.852;
const qreal KM2NM = 1.0 / NM2KM;
const qreal NM2FT = 6080; // nm feet

// Conversion Metric / Imperial System: meter vs. feet
const qreal M2FT = 3.2808;
const qreal FT2M = 1.0 / M2FT;

// Conversion Metric / Imperial System: meter vs inch
const qreal M2IN = 39.3701;
const qreal IN2M = 1.0 / M2IN;

// Interconversion between Imperial System: feet vs inch
const qreal FT2IN = 12.0;

// Conversion Metric / Imperial System: meter vs yard
const qreal M2YD = 1.09361;
const qreal YD2M = 1.0 / M2YD;

// Conversion meter vs millimeter
const qreal M2MM = 1000.0;
const qreal MM2M = 1.0 / M2MM;

// Conversion meter vs centimeter
const qreal M2CM = 100.0;
const qreal CM2M = 1.0 / M2CM;

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

const qreal TWOPI = 2 * M_PI;

// Version definitions to use with an external application (as digiKam)

// String for about dialog and http user agent
const QString MARBLE_VERSION_STRING = QString::fromLatin1( MARBLE_LIB_VERSION_STRING );

// API Version id:
// up until the 21.04 release, this was supposed to be 0xMMmmpp (major,minor,patch), but in reality it was stuck at version 0.27.0
// now it is  ((major<<16)|(minor<<8)|(patch))
#define MARBLE_VERSION MARBLE_LIB_VERSION

static const char NOT_AVAILABLE[] = QT_TRANSLATE_NOOP("Marble", "not available");

const int tileDigits = 6;

// Average earth radius in m
// Deprecated: Please use model()->planetRadius() instead.
const qreal EARTH_RADIUS = 6378137.0;

// Maximum level of base tiles
const int maxBaseTileLevel = 4;

// Default size (width and height) of tiles
const unsigned int c_defaultTileSize = 675;

class MarbleGlobalPrivate;
class MarbleLocale;

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

    /** @deprecated Profiles are detected automatically now. This only returns profiles() anymore */
    MARBLE_DEPRECATED static Profiles detectProfiles();
    
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
