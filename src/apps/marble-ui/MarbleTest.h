// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
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

 private:
    MarbleWidget      *m_marbleWidget;
};

}

#endif
