//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
//

#include "ElevationProfilePlotAxis.h"

#include "MarbleLocale.h"
#include "MarbleMath.h"

namespace Marble
{

ElevationProfilePlotAxis::ElevationProfilePlotAxis()
    : m_minValue( 0.0 ),
      m_maxValue ( 0.0 ),
      m_displayScale( 1.0 ),
      m_pixelLength ( 0 ),
      m_minTickCount( 2 ),
      m_maxTickCount( 5 ),
      m_unitString( QString() )
{
    // nothing to do...
}

void ElevationProfilePlotAxis::setRange( const qreal& minValue, const qreal& maxValue )
{
    m_minValue = minValue;
    m_maxValue = maxValue;
    update();
}

void ElevationProfilePlotAxis::setLength( const int& length )
{
    m_pixelLength = length;
    update();
}

void ElevationProfilePlotAxis::setTickCount( const int min, const int max )
{
    m_minTickCount = min;
    m_maxTickCount = max;
}

void ElevationProfilePlotAxis::update()
{
    updateTicks();
    updateScale();
}

qreal ElevationProfilePlotAxis::minValue()
{
    return m_minValue;
}

qreal ElevationProfilePlotAxis::maxValue()
{
    return m_maxValue;
}

qreal ElevationProfilePlotAxis::range()
{
    return m_maxValue - m_minValue;
}

qreal ElevationProfilePlotAxis::scale()
{
    return m_displayScale;
}

QString ElevationProfilePlotAxis::unit()
{
    return m_unitString;
}

AxisTickList ElevationProfilePlotAxis::ticks()
{
    return m_ticks;
}

void ElevationProfilePlotAxis::updateTicks()
{
    m_ticks.clear();
    if( range() == 0 ) {
        return;
    }

    QList<int> niceIntervals;
    niceIntervals << 10 << 20 << 25 << 30 << 50;

    const int exponent = round( log10( range() ) );
    const qreal factor = pow10( 2 - exponent );
    const qreal tickRange = range() * factor;

    qreal stepWidth = niceIntervals.last();
    qreal error = tickRange;
    foreach ( const int i, niceIntervals ) {
        const qreal numTicks = tickRange / i;
        if ( numTicks < m_minTickCount || numTicks > m_maxTickCount ) {
            continue;
        }
        const qreal newError = qAbs( numTicks - round( numTicks ) );
        if ( newError < error ) {
            error = newError;
            stepWidth = i;
        }
    }
    stepWidth /= factor;

    qreal offset = 0;
    if ( fmod( m_minValue, stepWidth ) != 0 ) {
        offset = stepWidth - fmod( m_minValue, stepWidth );
    }

    qreal val = m_minValue + offset;
    int pos = m_pixelLength / range() * offset;
    m_ticks << AxisTick( pos, val );
    while( val < m_maxValue ) {
        val += stepWidth;
        pos += m_pixelLength / range() * stepWidth;
        if ( pos > m_pixelLength ) {
            break;
        }
        m_ticks << AxisTick( pos, val );
    }
}

void ElevationProfilePlotAxis::updateScale()
{
    QLocale::MeasurementSystem measurementSystem;
    measurementSystem = MarbleGlobal::getInstance()->locale()->measurementSystem();
    switch ( measurementSystem ) {
    case QLocale::MetricSystem:
        if ( range() >= 10 * KM2METER ) {
            m_unitString = tr( "km" );
            m_displayScale = METER2KM;
        } else {
            m_unitString = tr( "m" );
            m_displayScale = 1.0;
        }
        break;
    case QLocale::ImperialSystem:
        // FIXME: Do these values make sense?
        if ( range() >= 10 * KM2METER * MI2KM ) {
            m_unitString = tr( "mi" );
            m_displayScale = METER2KM * KM2MI;
        } else {
            m_unitString = tr( "ft" );
            m_displayScale = M2FT;
        }
        break;
    }
}

}

#include "ElevationProfilePlotAxis.moc"