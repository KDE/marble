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


//
// AutoSettings automatically adjusts the display quality
//


#ifndef AUTOSETTINGS_H
#define AUTOSETTINGS_H

/** @file
 * This file contains the headers for AutoSettings.
 *
 * @author Torsten Rahn <tackat@kde.org>
 */

#include <QtCore/QObject>

class AutoSettings : public QObject
{
    Q_OBJECT

 public:
    AutoSettings( QObject *parent = 0 );
    ~AutoSettings();
};

#endif // AUTOSETTINGS_H
