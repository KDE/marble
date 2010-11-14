//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// Description: Some Tests for Marble
//

#ifndef MARBLE_MARBLETEST_H
#define MARBLE_MARBLETEST_H

namespace Marble
{

class MarbleWidget;

class MarbleTest
{
 public:
    explicit MarbleTest( MarbleWidget* marbleWidget );
    virtual ~MarbleTest(){ }

    void timeDemo();
    
    /**
     * @brief load a gpx file and test average, max, min, and total time
     */
    void gpsDemo();

 private:
    MarbleWidget      *m_marbleWidget;
};

}

#endif
