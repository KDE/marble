//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
//

#ifndef ELEVATIONPROFILEPLOTAXIS_H
#define ELEVATIONPROFILEPLOTAXIS_H

#include <QtCore/QObject>
#include <QList>
#include <QString>

namespace Marble
{

struct AxisTick {
    int position;
    qreal value;
    AxisTick( const int &position_, const qreal &value_ ) :
        position( position_ ),
        value( value_ )
    {
        // nothing to do
    }
};
typedef QList<AxisTick> AxisTickList;

 /**
 * @short A helper class handling a plot axis for the Elevation Profile
 *
 */
class ElevationProfilePlotAxis : public QObject
{
    Q_OBJECT
  private:
    qreal m_minValue;
    qreal m_maxValue;
    qreal m_displayScale;
    int m_pixelLength;
    int m_minTickCount;
    int m_maxTickCount;
    QString m_unitString;
    AxisTickList m_ticks;

    void updateTicks();
    void updateScale();

  public:
    explicit ElevationProfilePlotAxis( );
    void setRange( const qreal &minValue, const qreal &maxValue );
    void setLength( const int &length );
    void setTickCount( const int min, const int max );
    void update();
    qreal minValue();
    qreal maxValue();
    qreal range();
    qreal scale();
    QString unit();
    AxisTickList ticks();
};

}
#endif // ELEVATIONPROFILEPLOTAXIS_H
