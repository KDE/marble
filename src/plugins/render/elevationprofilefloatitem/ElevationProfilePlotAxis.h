// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
//

#ifndef ELEVATIONPROFILEPLOTAXIS_H
#define ELEVATIONPROFILEPLOTAXIS_H

#include <QObject>
#include <QVector>
#include <QString>

namespace Marble
{

struct AxisTick {
    int position;
    qreal value;
    AxisTick() :
        position(0),
        value(0.0)
    {
    }
    AxisTick(int position_, qreal value_) :
        position( position_ ),
        value( value_ )
    {
        // nothing to do
    }
};
using AxisTickList = QVector<AxisTick>;

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
    void setRange(qreal minValue, qreal maxValue);
    void setLength(int length);
    void setTickCount( const int min, const int max );
    void update();
    qreal minValue() const;
    qreal maxValue() const;
    qreal range() const;
    qreal scale() const;
    QString unit() const;
    AxisTickList ticks() const;
};

}

Q_DECLARE_TYPEINFO(Marble::AxisTick, Q_PRIMITIVE_TYPE);

#endif // ELEVATIONPROFILEPLOTAXIS_H
