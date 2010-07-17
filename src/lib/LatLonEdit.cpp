//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "LatLonEdit.h"
#include "ui_LatLonEdit.h"

#include <QtGui/QWidget>
#include "MarbleDebug.h"

#include <cstdlib>

#include "global.h"

using namespace Marble;
namespace Marble
{

class LatLonEditPrivate : public Ui::LatLonEditPrivate
{
public:
    Dimension m_dimension;
    qreal m_value;

    LatLonEditPrivate(QWidget* parent) { setupUi(parent); }
};

}

LatLonEdit::LatLonEdit(QWidget *parent, Dimension dimension )
    : QWidget( parent ), d(new LatLonEditPrivate(this))
{
    d->m_value = 0;
    setDimension( dimension);

    connect(d->m_sec, SIGNAL(valueChanged(int)), this, SLOT(secOverflow()));
    connect(d->m_min, SIGNAL(valueChanged(int)), this, SLOT(minOverflow()));
    connect(d->m_deg, SIGNAL(valueChanged(int)), this, SLOT(recalculate()));

    connect(d->m_sec, SIGNAL(valueChanged(int)), this, SLOT(checkSign()));
    connect(d->m_min, SIGNAL(valueChanged(int)), this, SLOT(checkSign()));
    connect(d->m_deg, SIGNAL(valueChanged(int)), this, SLOT(checkSign()));

    connect(d->m_sign, SIGNAL(currentIndexChanged(int)),
                 this, SLOT(signChanged()));
}

LatLonEdit::~LatLonEdit()
{
    delete d;
}

qreal LatLonEdit::value() const
{
    return d->m_value;
}

void LatLonEdit::checkSign()
{
    recalculate();
    if( d->m_value < 0 ) {
        d->m_sign->setEnabled(true);
        d->m_sign->setCurrentIndex( 1 );
    } else if( d->m_value > 0 ) {
        d->m_sign->setEnabled(true);
        d->m_sign->setCurrentIndex( 0 );
    } else {
        //d->m_value is zero, so long/lat do not apply
        d->m_sign->setEnabled(false);
    }
}

void LatLonEdit::signChanged()
{
    /* Only flip the value if they disagree */
    if( d->m_sign->currentIndex() == 0 && d->m_value < 0 ) {
        mDebug() << "d->m_sign->currentIndex() == 0 && d->m_value < 0";
        d->m_value *= -1;
    } else if( d->m_sign->currentIndex() == 1 && d->m_value > 0 ) {
        mDebug() << "d->m_sign->currentIndex() == 1 && d->m_value > 0";
        d->m_value *= -1;
    }
}

void LatLonEdit::setDimension( Dimension dimension )
{
    //don't do anything
    if( d->m_dimension == dimension ) {
        return;
    }

    if( dimension == Longitude ) {
        for(int i = 0; i < d->m_sign->count(); ++i)
            d->m_sign->removeItem(i);
        d->m_deg->setMinimum( -180 );
        d->m_deg->setMaximum( 180 );
        d->m_sign->addItem( tr("E", "East, the direction" ) );
        d->m_sign->addItem( tr("W", "West, the direction" ) );
    } else if( dimension == Latitude ) {
        for(int i = 0; i < d->m_sign->count(); ++i)
            d->m_sign->removeItem(i);
        d->m_deg->setMinimum( -90 );
        d->m_deg->setMaximum( 90 );
        d->m_sign->addItem( tr("N", "North, the direction" ) );
        d->m_sign->addItem( tr("S", "South, the direction" ) );
    } else {
        mDebug() << "Unrecognized dimension" << dimension;
        for(int i = 0; i < d->m_sign->count(); ++i)
            d->m_sign->removeItem(i);
        d->m_deg->setMinimum( -32768 );
        d->m_deg->setMaximum( 32767 );
        d->m_sign->addItem( "+" );
        d->m_sign->addItem( "-" );
    }

}

void LatLonEdit::secOverflow()
{
    //you can't have 180°59'59" , only 180.
    if( d->m_deg->value() == d->m_deg->maximum() ) {
        d->m_min->setValue( 0 );
        d->m_sec->setValue( 0 );
        recalculate();
        return;
    } else if( d->m_sec->value() == 60 ) {
        d->m_sec->setValue( 0 );
        d->m_min->setValue( d->m_min->value() + 1 );
        recalculate();
        return;
    } else if( d->m_sec->value() == -1 ) {
        //this handles the case of going from 0°0'0" to 0°0'0" instead
        //              of letting it go from 0°0'0" to 1°59'59"
        if( d->m_deg->value() == 0 && d->m_min->value() == 0 ) {
            d->m_sec->setValue( 1 );
            d->m_sign->setCurrentIndex(d->m_sign->currentIndex() == 1 ? 0:1);
        } else {
            d->m_sec->setValue( 59 );
            d->m_min->setValue( d->m_min->value() - 1 );
        }
        recalculate();
        return;
    }
}

void LatLonEdit::minOverflow()
{
    //you can't have 180°59'59" , only 180.
    if( d->m_deg->value() == d->m_deg->maximum() ) {
        d->m_min->setValue( 0 );
        d->m_sec->setValue( 0 );
        recalculate();
        return;
    } else if( d->m_min->value() == 60 ) {
        d->m_min->setValue( 0 );
        d->m_deg->setValue( d->m_deg->value() + 1 );
        recalculate();
        return;
    } else if( d->m_min->value() == -1 ) {
        //this handles the case of going from 0°0'0" to 0°0'0" instead
        //              of letting it go from 0°0'0" to 1°59'0"
        if( d->m_deg->value() == 0 ) {
            d->m_min->setValue( 1 );
            d->m_sign->setCurrentIndex(d->m_sign->currentIndex() == 1 ? 0:1);
        } else {
            d->m_min->setValue( 59 );
            d->m_deg->setValue( d->m_deg->value() - 1 );
        }
        recalculate();
        return;
    }
}

void LatLonEdit::setValue( qreal newvalue )
{
    if( abs(newvalue) > (qreal)(d->m_deg->maximum()) )
        return; //out of bounds
    d->m_value = newvalue;
    reverseRecalculate();
    //mDebug() << "valueChanged: now " << d->m_value;
    emit valueChanged( d->m_value );
}

void LatLonEdit::recalculate()
{
    qreal deg = d->m_deg->value();
    qreal min = (qreal)(d->m_min->value()) / 60.0;
    qreal sec = (qreal)(d->m_sec->value()) / 3600.0;
    mDebug() << "Recalculate" << deg << min << sec;
    /*this is delayed in order to avoid false emissions of valueChanged */
    bool changesign = false;

    if( d->m_deg->value() < 0 ) {
        deg = abs(deg);
        changesign = true;
    }

    //if the degrees is neg, the mins/secs *subtract* from the value
    if( d->m_sign->currentIndex() ) {
        d->m_value = 0 - deg - min - sec;
    } else {
        d->m_value = deg + min + sec;
    }

    if(changesign) {
        d->m_sign->setCurrentIndex(d->m_sign->currentIndex() == 1 ? 0:1);
        d->m_deg->setValue( abs(d->m_deg->value()) );
        mDebug() << "setval " << abs(d->m_deg->value()) << "d->m_sign->setEnabled(true);";
        d->m_sign->setEnabled(true);
    }
    mDebug() << "m_value = " << d->m_value;
    emit valueChanged( d->m_value );
}


void LatLonEdit::reverseRecalculate()
{
    int dv, mv, sv;
    d->m_sign->currentIndex() ? dv = ceil(d->m_value): dv = floor(d->m_value);

    qreal mr = d->m_value - dv; //minutes remainder
    d->m_sign->currentIndex() ? mv = ceil(mr *60) : mv = floor(mr *60);

    qreal sr = mr - mv; //seconds remainder
    d->m_sign->currentIndex() ? sv = ceil(sr *60) : sv = floor(sr *60);

    d->m_deg->setValue( abs(dv) );
    d->m_min->setValue( abs(mv) );
    d->m_sec->setValue( abs(sv) );

    checkSign();
}

#include "LatLonEdit.moc"



