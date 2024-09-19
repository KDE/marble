// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011, 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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
    explicit FpsLayer(QElapsedTimer *time);

    void paint(QPainter *painter) const;

private:
    QElapsedTimer *const m_time;
};

}

#endif
