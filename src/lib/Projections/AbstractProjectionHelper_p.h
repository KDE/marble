//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>"
//


#ifndef ABSTRACTPROJECTIONHELPER_P_H
#define ABSTRACTPROJECTIONHELPER_P_H


/** @file
 * This file contains the headers for AbstractProjectionPrivate.
 *
 * @author Inge Wallin  <inge@lysator.liu.se>
 */


// Qt
#include <QtGui/QRegion>


class AbstractProjectionHelperPrivate
{
 public:
    QRegion  activeRegion;
};




#endif // ABSTRACTPROJECTIONHELPER_P_H
