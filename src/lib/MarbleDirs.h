//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef MARBLEDIRS_H
#define MARBLEDIRS_H


#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtGui/QApplication>


#ifdef Q_OS_MACX
//for getting app bundle path
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <config-marble.h>

/**
 * @short A class that manages data look-up for Marble.
 *
 * The class basically does for Marble what KStandardDirs does for KDE.
 * Given that the MarbleWidget is a Qt4-only library and given that it 
 * comes with its own model and data we need this class.
 *
 * The class needs to respect the requirements of the different plattforms,
 * so to avoid very different implementations for each plattform we 
 * specify how data should get looked up:
 * 
 * Generally there are two places of which Marble will draw it's data from:
 * 
 * "localDir" and "systemDir".
 * 
 * look-up of the data should happen in the localDir first.
 * Only if the look-up in the localDir failed then MarbleDirs should 
 * look up the data in the systemDir.
 * 
 * homeDir:
 * The place for homeDir should match space that is fully accessible to 
 * the user. On Unix-like plattforms this matches 
 * QDir::homePath() + "/.marble/data"
 * 
 * systemDir:
 * Ideally the systemDir should match the place where cmake installed the 
 * data for marble. However this doesn't work for all plattforms:
 * 
 * - For Linux and Mac Non-bundle deployment the location can be 
 *   chosen using the cmake MARBLE_DATA_PATH option at compile time.
 * - For Mac bundle deployment the location inside the bundle gets
 *   chosen as the default location.
 * - For Windows a path relative to the application binary can be chosen
 *   as this should usually work without problems.
 * 
 * To allow kiosk-like setups and for custom setups in general
 * it should be possible to change the place of the systemDir at runtime. 
 * Therefore we introduce a global variable "MarbleDataPath" in the 
 * MarbleDirs.h source code.
 * Initially MarbleDataPath is empty. The systemDir will point to 
 * MarbleDataPath as soon as it gets changed to a valid non-empty path. So 
 * as soon as MarbleDataPath contains a valid path the path specified by 
 * cmake will get ignored.
 *
 * It's the duty of each application that uses the MarbleWidget to retrieve 
 * the value of the MarbleDataPath from the MarbleWidget and to save it 
 * in its settings and restore it on start-up of the application.
 * 
 */

class MarbleDirs
{
 public:
    static QString path( const QString& path );

    static QString systemDir(); 

    static QString localDir(); 
};


#endif // MARBLEDIRS_H
