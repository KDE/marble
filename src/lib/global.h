//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef GLOBAL_H
#define GLOBAL_H


#include <math.h>

#include <QtCore/QString>

#include "marble_export.h"

namespace Marble
{

enum TessellationFlag {
    NoTessellation = 0x0,
    Tessellate = 0x1, 
    RespectLatitudeCircle = 0x2,
    FollowGround = 0x4,
    RotationIndicatesFill = 0x8
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
 * @brief This enum is used to choose the unit chosen to measure distances.
 */
enum DistanceUnit {
    Metric,             ///< Metric unit
    Imperial            ///< Imperial unit
};

/**
 * @brief This enum is used to choose the dimension.
 */
enum Dimension {
    Latitude,             ///< Latitude
    Longitude            ///< Longitude
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
    Outline,            ///< Only a wire representation is drawn
    Low,                ///< Low resolution (e.g. interlaced)
    Normal,             ///< Normal quality
    High,               ///< High quality (e.g. antialiasing for lines)
    Print               ///< Print quality
}; 

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

const int defaultLevelZeroColumns = 2;
const int defaultLevelZeroRows = 1;

// Conversion Metric / Imperial System: km vs. miles
const qreal MI2KM = 1.609344;
const qreal KM2MI = 1.0 / MI2KM;

// Conversion degree vs. radians
const qreal DEG2RAD = M_PI / 180.0;
const qreal RAD2DEG = 180.0 / M_PI;


// Version definitions to use with an external application (as digiKam)

// String for about dialog.
const QString MARBLE_VERSION_STRING = QString::fromLatin1( "0.8SVN" );

// API Version id:
// form : 0xMMmmpp
//        MM = major revision.
//        mm = minor revision.
//        pp = patch revision.
#define MARBLE_VERSION 0x000800

static const char NOT_AVAILABLE[] = QT_TR_NOOP("not available");

const int tileDigits = 6;

// Usage deprecated:
const qreal RAD2INT = 21600.0 / M_PI;

// Average earth radius in m
// Deprecated: Please use model()->planetRadius() instead.
const qreal EARTH_RADIUS = 6378000.0;

class MarbleGlobalPrivate;
class MarbleLocale;

class  MARBLE_EXPORT MarbleGlobal {
 public:
    static MarbleGlobal * getInstance();
    ~MarbleGlobal();

    MarbleLocale * locale() const;

 private:
    MarbleGlobal();

    Q_DISABLE_COPY( MarbleGlobal )
    MarbleGlobalPrivate  * const d;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Marble::TessellationFlags)

#endif
