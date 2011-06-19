//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_ALTITUDE_PLOTWIDGET_H
#define MARBLE_ALTITUDE_PLOTWIDGET_H

#include <KPlotWidget>

namespace Marble {

class AltitudeProfile;
class PlotPoint;

class PlotWidget : public KPlotWidget
{
    Q_OBJECT
public:
    explicit PlotWidget( AltitudeProfile * parent = 0 );

    void clearHighligtedPoint();
    PlotPoint *highlightedPoint() const { return m_highligtedPoint; }

protected:
    virtual void mouseMoveEvent(QMouseEvent* );
    virtual void paintEvent(QPaintEvent* );

private:
    AltitudeProfile *m_altitudeProfile;
    PlotPoint *m_highligtedPoint;
};

}

#endif
