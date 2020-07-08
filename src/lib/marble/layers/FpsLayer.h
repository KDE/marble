//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011,2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_FPSLAYER_H
#define MARBLE_FPSLAYER_H

class QPainter;
class QElapsedTimer;

namespace Marble
{

class FpsLayer
{
public:
    explicit FpsLayer( QElapsedTimer *time );

    void paint( QPainter *painter ) const;

private:
    QElapsedTimer *const m_time;
};

}

#endif
