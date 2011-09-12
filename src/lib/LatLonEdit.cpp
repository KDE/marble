//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>


#include "LatLonEdit.h"
#include "ui_LatLonEdit.h"

#include <QtGui/QWidget>
#include "MarbleDebug.h"

#include "global.h"

using namespace Marble;
namespace Marble
{

static void
switchSign( QComboBox *sign )
{
    sign->setCurrentIndex( (sign->currentIndex() == 1) ? 0 : 1 );
}


class LatLonEditPrivate : public Ui::LatLonEditPrivate
{
public:
    Dimension m_dimension;
    qreal m_value;
    // flag which indicates that the widgets are updated due to a change
    // in one of the other widgets. Q*SpinBox does not have a signal which is
    // only emitted by a change due to user input, not code setting a new value.
    // This flag should be less expensive then disconnecting from and reconnecting
    // to the valueChanged signal of all widgets.
    bool m_updating : 1;

    LatLonEditPrivate() : m_value(0.0), m_updating( false ) {}
    void init(QWidget* parent) { setupUi(parent); }
};

}

LatLonEdit::LatLonEdit(QWidget *parent, Dimension dimension )
    : QWidget( parent ), d(new LatLonEditPrivate())
{
    d->init(this);
    setDimension( dimension);

    connect(d->m_sec, SIGNAL(valueChanged(double)), this, SLOT(checkSecOverflow()));
    connect(d->m_min, SIGNAL(valueChanged(int)),    this, SLOT(checkMinOverflow()));
    connect(d->m_deg, SIGNAL(valueChanged(int)),    this, SLOT(checkDegOverflow()));

    connect(d->m_sign, SIGNAL(currentIndexChanged(int)),
                 this, SLOT(onSignChanged()));
}

LatLonEdit::~LatLonEdit()
{
    delete d;
}

qreal LatLonEdit::value() const
{
    return d->m_value;
}

void LatLonEdit::onSignChanged()
{
    if( d->m_updating )
        return;

    // Only flip the value if it does not match the sign
    if( d->m_sign->currentIndex() == 0 ) {
        if( d->m_value < 0 ) {
            d->m_value *= -1;
        }
    } else {
        if( d->m_value > 0 ) {
            d->m_value *= -1;
        }
    }

    emit valueChanged( d->m_value );
}

void LatLonEdit::setDimension( Dimension dimension )
{
    // should not happen
    if( dimension != Longitude && dimension != Latitude ) {
        mDebug() << "Unrecognized dimension" << dimension;
        return;
    }

    //don't do anything
    if( d->m_dimension == dimension ) {
        return;
    }

    d->m_updating = true;

    d->m_sign->clear();

    if( dimension == Longitude ) {
        d->m_deg->setMinimum( -180 );
        d->m_deg->setMaximum(  180 );

        d->m_sign->addItem( tr("E", "East, the direction" ) );
        d->m_sign->addItem( tr("W", "West, the direction" ) );
    } else if( dimension == Latitude ) {
        d->m_deg->setMinimum( -90 );
        d->m_deg->setMaximum(  90 );

        d->m_sign->addItem( tr("N", "North, the direction" ) );
        d->m_sign->addItem( tr("S", "South, the direction" ) );
    }

    d->m_dimension = dimension;

    d->m_updating = false;

    // reset value, old one is useless
    setValue( 0.0 );
}

void LatLonEdit::checkSecOverflow()
{
    if( d->m_updating )
        return;

    d->m_updating = true;

    const int degValue = d->m_deg->value();
    const int minValue = d->m_min->value();
    const qreal secValue = d->m_sec->value();

    if( secValue < 0.0 ) {
        const qreal secDiff = -secValue;
        if( degValue == 0 && minValue == 0 ) {
            switchSign( d->m_sign );
            d->m_sec->setValue( secDiff );
        } else {
            d->m_sec->setValue( 60.0 - secDiff );
            if( minValue > 0 ) {
                d->m_min->setValue( minValue - 1 );
            } else {
                d->m_min->setValue( 59 );
                const int degDec = (degValue > 0) ? 1 : -1;
                d->m_deg->setValue( degValue - degDec );
            }
        }
    } else {
        const int minDegValue = d->m_deg->minimum();
        const int maxDegValue = d->m_deg->maximum();
        // at max/min already?
        if( degValue <= minDegValue || maxDegValue <= degValue ) {
            // ignore
            d->m_sec->setValue( 0.0 );
        // need to inc minutes?
        } else if( secValue >= 60.0 ) {
            qreal newSec = secValue - 60.0;
            // will reach max/min?
            if( minValue == 59 ) {
                d->m_min->setValue( 0 );
                // will reach max/min?
                if( minDegValue+1 == degValue || degValue == maxDegValue-1 ) {
                    // reset also sec
                    newSec = 0.0;
                }
                const int degInc = (degValue > 0) ? 1 : -1;
                d->m_deg->setValue( degValue + degInc );
            } else {
                d->m_min->setValue( minValue + 1 );
            }
            d->m_sec->setValue( newSec );
        }
    }

    d->m_updating = false;

    recalculate();
}


void LatLonEdit::checkMinOverflow()
{
    if( d->m_updating )
        return;

    d->m_updating = true;

    const int degValue = d->m_deg->value();
    const int minValue = d->m_min->value();

    if( minValue < 0 ) {
        if( degValue != 0 ) {
            d->m_min->setValue( 59 );
            const int degDec = (degValue > 0) ? 1 : -1;
            d->m_deg->setValue( degValue - degDec );
        } else {
            switchSign( d->m_sign );
            d->m_min->setValue( 1 );
        }
    } else {
        const int minDegValue = d->m_deg->minimum();
        const int maxDegValue = d->m_deg->maximum();
        // at max/min already?
        if( degValue <= minDegValue || maxDegValue <= degValue ) {
            // ignore
            d->m_min->setValue( 0 );
        // overflow?
        } else if( minValue >= 60 ) {
            d->m_min->setValue( 0 );
            // will reach max/min?
            if( minDegValue+1 == degValue || degValue == maxDegValue-1 ) {
                // reset also sec
                d->m_sec->setValue( 0.0 );
            }
            const int degInc = (degValue > 0) ? 1 : -1;
            d->m_deg->setValue( degValue + degInc );
        }
    }

    d->m_updating = false;

    recalculate();
}

void LatLonEdit::checkDegOverflow()
{
    if( d->m_updating )
        return;

    d->m_updating = true;

    const int degValue = d->m_deg->value();
    const int minDegValue = d->m_deg->minimum();
    const int maxDegValue = d->m_deg->maximum();
    // at max/min?
    if( degValue <= minDegValue || maxDegValue <= degValue ) {
        d->m_min->setValue( 0 );
        d->m_sec->setValue( 0.0 );
    }

    d->m_updating = false;

    recalculate();
}

void LatLonEdit::setValue( qreal newvalue )
{
    // limit to allowed values
    const qreal maxCoordValue = (qreal)( d->m_deg->maximum() );
    const qreal minCoordValue = (qreal)( d->m_deg->minimum() );
    if( newvalue > maxCoordValue ) {
        newvalue = maxCoordValue;
    } else if( newvalue < minCoordValue ) {
        newvalue = minCoordValue;
    }

    // no change?
    if( newvalue == d->m_value ) {
        return;
    }

    d->m_value = newvalue;

    // calculate sub values
    // calculation is done similar to GeoDataCoordinates::lonToString,
    // perhaps should be moved with similar methods into some utility class/namespace

    // split value and sign
    const bool isNegative = ( newvalue < 0.0 );
    newvalue = qAbs( newvalue );

    int degValue = newvalue;

    qreal lonMinF = 60 * (newvalue - degValue);
    int minValue = (int) lonMinF;
    qreal secFValue = 60 * (lonMinF - minValue);
    // Adjustment for fuzziness (like 49.999999999999999999999)
    int secValue = qRound( secFValue );
    if (secValue > 59) {
        secFValue = 0.0;
        ++minValue;
    }
    if (minValue > 59) {
        minValue = 0;
        ++degValue;
    }

    // update widgets
    d->m_updating = true;

    d->m_deg->setValue( degValue );
    d->m_min->setValue( minValue );
    d->m_sec->setValue( secFValue );
    d->m_sign->setCurrentIndex( isNegative ? 1 : 0 );

    d->m_updating = false;
}

void LatLonEdit::recalculate()
{
    const bool isNegativeDeg = ( d->m_deg->value() < 0 );

    const qreal deg = (qreal)(qAbs(d->m_deg->value()));
    const qreal min = (qreal)(d->m_min->value()) / 60.0;
    const qreal sec = d->m_sec->value() / 3600.0;

    qreal value = deg + min + sec;

    if( isNegativeDeg )
        value *= -1;
    if( d->m_sign->currentIndex() == 1 )
        value *= -1;

    d->m_value = value;

    emit valueChanged( d->m_value );
}


#include "LatLonEdit.moc"
