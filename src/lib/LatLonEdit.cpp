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

#include "MarbleGlobal.h"


namespace Marble
{

// This widget can have 3 different designs, one per notation (Decimal, DMS, DM)
// To reduce the footprint this was not implemented using a stack of widgets
// where each widget offer the needed design for another notation.
// Instead, as Decimal and DM are both using subsets of the UI elements used
// for DMS, just the UI elements for DMS are created and modified as needed,
// if another notation is selected. This involves showing and hiding them and
// setting the proper suffix and min/max values.
// The logic per notation is moved into specialized subclasses of a class 
// AbstractInputHandler.
// TODO: simply remove the LatLonEdit.ui file and embed code directly here?

enum { PositiveSphereIndex = 0, NegativeSphereIndex = 1 };


class LatLonEditPrivate;

class AbstractInputHandler // TODO: better name
{
protected:
    AbstractInputHandler(LatLonEditPrivate *ui) : m_ui(ui) {}
public:
    virtual ~AbstractInputHandler() {}

public: // API to be implemented
    virtual void setupUi() = 0;
    virtual void setupMinMax(Dimension dimension) = 0;
    virtual void setValue(qreal value) = 0;
    virtual void handleIntEditChange() = 0;
    virtual void handleUIntEditChange() = 0;
    virtual void handleFloatEditChange() = 0;
    virtual qreal calculateValue() const = 0;

protected:
    LatLonEditPrivate * const m_ui;
};

class DecimalInputHandler : public AbstractInputHandler
{
public:
    DecimalInputHandler(LatLonEditPrivate *ui) : AbstractInputHandler(ui) {}
public: // AbstractInputHandler API
    virtual void setupUi();
    virtual void setupMinMax(Dimension dimension);
    virtual void setValue(qreal value);
    virtual void handleIntEditChange();
    virtual void handleUIntEditChange();
    virtual void handleFloatEditChange();
    virtual qreal calculateValue() const;
};

class DMSInputHandler : public AbstractInputHandler
{
public:
    DMSInputHandler(LatLonEditPrivate *ui) : AbstractInputHandler(ui) {}
public: // AbstractInputHandler API
    virtual void setupUi();
    virtual void setupMinMax(Dimension dimension);
    virtual void setValue(qreal value);
    virtual void handleIntEditChange();
    virtual void handleUIntEditChange();
    virtual void handleFloatEditChange();
    virtual qreal calculateValue() const;
};

class DMInputHandler : public AbstractInputHandler
{
public:
    DMInputHandler(LatLonEditPrivate *ui) : AbstractInputHandler(ui) {}
public: // AbstractInputHandler API
    virtual void setupUi();
    virtual void setupMinMax(Dimension dimension);
    virtual void setValue(qreal value);
    virtual void handleIntEditChange();
    virtual void handleUIntEditChange();
    virtual void handleFloatEditChange();

    virtual qreal calculateValue() const;
};

class LatLonEditPrivate : public Ui::LatLonEditPrivate
{
    friend class DecimalInputHandler;
    friend class DMSInputHandler;
    friend class DMInputHandler;

public:
    Dimension m_dimension;
    qreal m_value;
    GeoDataCoordinates::Notation m_notation;
    AbstractInputHandler *m_inputHandler;
    // flag which indicates that the widgets are updated due to a change
    // in one of the other widgets. Q*SpinBox does not have a signal which is
    // only emitted by a change due to user input, not code setting a new value.
    // This flag should be less expensive then disconnecting from and reconnecting
    // to the valueChanged signal of all widgets.
    bool m_updating : 1;

    LatLonEditPrivate();
    ~LatLonEditPrivate();
    void init(QWidget* parent);
};


static void
switchSign( QComboBox *sign )
{
    const bool isNegativeSphere = (sign->currentIndex() == NegativeSphereIndex);
    sign->setCurrentIndex( isNegativeSphere ? PositiveSphereIndex : NegativeSphereIndex );
}

void DecimalInputHandler::setupUi()
{
    m_ui->m_floatValueEditor->setSuffix(LatLonEdit::trUtf8("\xC2\xB0")); // the degree symbol °
    m_ui->m_floatValueEditor->setDecimals(5);

    m_ui->m_intValueEditor->hide();
    m_ui->m_uintValueEditor->hide();
}

void DecimalInputHandler::setupMinMax(Dimension dimension)
{
    const qreal maxValue = (dimension == Longitude) ? 180.0 : 90.0;

    m_ui->m_floatValueEditor->setMinimum(-maxValue);
    m_ui->m_floatValueEditor->setMaximum( maxValue);
}

void DecimalInputHandler::setValue(qreal value)
{
    value = qAbs(value);

    m_ui->m_floatValueEditor->setValue(value);
}

void DecimalInputHandler::handleIntEditChange()
{
    // nothing to do, perhaps rather disconnect the signal with this notation
}

void DecimalInputHandler::handleUIntEditChange()
{
    // nothing to do, perhaps rather disconnect the signal with this notation
}

void DecimalInputHandler::handleFloatEditChange()
{
    // nothing to do, perhaps rather disconnect the signal with this notation
}

qreal DecimalInputHandler::calculateValue() const
{
    qreal value = m_ui->m_floatValueEditor->value();

    if (m_ui->m_sign->currentIndex() == NegativeSphereIndex) {
        value *= -1;
    }

    return value;
}

void DMSInputHandler::setupUi()
{
    m_ui->m_uintValueEditor->setSuffix(LatLonEdit::tr("'"));
    m_ui->m_floatValueEditor->setSuffix(LatLonEdit::tr("\""));
    m_ui->m_floatValueEditor->setDecimals(2);

    m_ui->m_intValueEditor->show();
    m_ui->m_uintValueEditor->show();
}

void DMSInputHandler::setupMinMax(Dimension dimension)
{
    const int maxValue = (dimension == Longitude) ? 180 : 90;

    m_ui->m_intValueEditor->setMinimum(-maxValue);
    m_ui->m_intValueEditor->setMaximum( maxValue);
}

void DMSInputHandler::setValue(qreal value)
{
    value = qAbs( value );

    int degValue = (int) value;

    qreal minFValue = 60 * (value - degValue);
    int minValue = (int) minFValue;
    qreal secFValue = 60 * (minFValue - minValue);
    // Adjustment for fuzziness (like 49.999999999999999999999)
    int secValue = qRound(secFValue);
    if (secValue > 59) {
        secFValue = 0.0;
        ++minValue;
    }
    if (minValue > 59) {
        minValue = 0;
        ++degValue;
    }

    m_ui->m_intValueEditor->setValue( degValue );
    m_ui->m_uintValueEditor->setValue( minValue );
    m_ui->m_floatValueEditor->setValue( secFValue );
}

void DMSInputHandler::handleIntEditChange()
{
    const int degValue = m_ui->m_intValueEditor->value();
    const int minDegValue = m_ui->m_intValueEditor->minimum();
    const int maxDegValue = m_ui->m_intValueEditor->maximum();
    // at max/min?
    if (degValue <= minDegValue || maxDegValue <= degValue) {
        m_ui->m_uintValueEditor->setValue( 0 );
        m_ui->m_floatValueEditor->setValue( 0.0 );
    }
}

void DMSInputHandler::handleUIntEditChange()
{
    const int degValue = m_ui->m_intValueEditor->value();
    const int minValue = m_ui->m_uintValueEditor->value();

    if (minValue < 0) {
        if (degValue != 0) {
            m_ui->m_uintValueEditor->setValue( 59 );
            const int degDec = (degValue > 0) ? 1 : -1;
            m_ui->m_intValueEditor->setValue( degValue - degDec );
        } else {
            switchSign( m_ui->m_sign );
            m_ui->m_uintValueEditor->setValue( 1 );
        }
    } else {
        const int minDegValue = m_ui->m_intValueEditor->minimum();
        const int maxDegValue = m_ui->m_intValueEditor->maximum();
        // at max/min already?
        if (degValue <= minDegValue || maxDegValue <= degValue) {
            // ignore
            m_ui->m_uintValueEditor->setValue( 0 );
        // overflow?
        } else if (minValue >= 60) {
            m_ui->m_uintValueEditor->setValue( 0 );
            // will reach max/min?
            if (minDegValue+1 == degValue || degValue == maxDegValue-1) {
                // reset also sec
                m_ui->m_floatValueEditor->setValue( 0.0 );
            }
            const int degInc = (degValue > 0) ? 1 : -1;
            m_ui->m_intValueEditor->setValue( degValue + degInc );
        }
    }
}

void DMSInputHandler::handleFloatEditChange()
{
    const int degValue = m_ui->m_intValueEditor->value();
    const int minValue = m_ui->m_uintValueEditor->value();
    const qreal secValue = m_ui->m_floatValueEditor->value();

    if (secValue < 0.0) {
        const qreal secDiff = -secValue;
        if (degValue == 0 && minValue == 0) {
            switchSign( m_ui->m_sign );
            m_ui->m_floatValueEditor->setValue( secDiff );
        } else {
            m_ui->m_floatValueEditor->setValue( 60.0 - secDiff );
            if (minValue > 0) {
                m_ui->m_uintValueEditor->setValue( minValue - 1 );
            } else {
                m_ui->m_uintValueEditor->setValue( 59 );
                const int degDec = (degValue > 0) ? 1 : -1;
                m_ui->m_intValueEditor->setValue( degValue - degDec );
            }
        }
    } else {
        const int minDegValue = m_ui->m_intValueEditor->minimum();
        const int maxDegValue = m_ui->m_intValueEditor->maximum();
        // at max/min already?
        if (degValue <= minDegValue || maxDegValue <= degValue) {
            // ignore
            m_ui->m_floatValueEditor->setValue( 0.0 );
        // need to inc minutes?
        } else if (secValue >= 60.0) {
            qreal newSec = secValue - 60.0;
            // will reach max/min?
            if (minValue == 59) {
                m_ui->m_uintValueEditor->setValue( 0 );
                // will reach max/min?
                if (minDegValue+1 == degValue || degValue == maxDegValue-1) {
                    // reset also sec
                    newSec = 0.0;
                }
                const int degInc = (degValue > 0) ? 1 : -1;
                m_ui->m_intValueEditor->setValue( degValue + degInc );
            } else {
                m_ui->m_uintValueEditor->setValue( minValue + 1 );
            }
            m_ui->m_floatValueEditor->setValue( newSec );
        }
    }
}

qreal DMSInputHandler::calculateValue() const
{
    const bool isNegativeDeg = ( m_ui->m_intValueEditor->value() < 0 );

    const qreal deg = (qreal)(qAbs(m_ui->m_intValueEditor->value()));
    const qreal min = (qreal)(m_ui->m_uintValueEditor->value()) / 60.0;
    const qreal sec = m_ui->m_floatValueEditor->value() / 3600.0;

    qreal value = deg + min + sec;

    if (isNegativeDeg) {
        value *= -1;
    }
    if (m_ui->m_sign->currentIndex() == NegativeSphereIndex) {
        value *= -1;
    }

    return value;
}

void DMInputHandler::setupUi()
{
    m_ui->m_floatValueEditor->setSuffix(LatLonEdit::tr("'"));
    m_ui->m_floatValueEditor->setDecimals(2);

    m_ui->m_intValueEditor->show();
    m_ui->m_uintValueEditor->hide();
}

void DMInputHandler::setupMinMax(Dimension dimension)
{
    const int maxValue = (dimension == Longitude) ? 180 : 90;

    m_ui->m_intValueEditor->setMinimum(-maxValue);
    m_ui->m_intValueEditor->setMaximum( maxValue);
}

void DMInputHandler::setValue(qreal value)
{
    value = qAbs(value);

    int degValue = (int)value;

    qreal minFValue = 60 * (value - degValue);
    // Adjustment for fuzziness (like 49.999999999999999999999)
    int minValue = qRound( minFValue );
    if (minValue > 59) {
        minFValue = 0.0;
        ++degValue;
    }

    m_ui->m_intValueEditor->setValue( degValue );
    m_ui->m_floatValueEditor->setValue( minFValue );
}

void DMInputHandler::handleIntEditChange()
{
    const int degValue = m_ui->m_intValueEditor->value();
    const int minDegValue = m_ui->m_intValueEditor->minimum();
    const int maxDegValue = m_ui->m_intValueEditor->maximum();
    // at max/min?
    if (degValue <= minDegValue || maxDegValue <= degValue) {
        m_ui->m_floatValueEditor->setValue( 0.0 );
    }
}

void DMInputHandler::handleUIntEditChange()
{
    // nothing to be done here, should be never called
}

void DMInputHandler::handleFloatEditChange()
{
    const int degValue = m_ui->m_intValueEditor->value();
    const qreal minValue = m_ui->m_floatValueEditor->value();

    if (minValue < 0.0) {
        const qreal minDiff = -minValue;
        if (degValue == 0) {
            switchSign( m_ui->m_sign );
            m_ui->m_floatValueEditor->setValue( minDiff );
        } else {
            m_ui->m_floatValueEditor->setValue( 60.0 - minDiff );
            m_ui->m_intValueEditor->setValue( degValue - 1 );
        }
    } else {
        const int minDegValue = m_ui->m_intValueEditor->minimum();
        const int maxDegValue = m_ui->m_intValueEditor->maximum();
        // at max/min already?
        if (degValue <= minDegValue || maxDegValue <= degValue) {
            // ignore
            m_ui->m_floatValueEditor->setValue( 0.0 );
        // need to inc degrees?
        } else if (minValue >= 60.0) {
            qreal newMin = minValue - 60.0;
            // will reach max/min?
            if (minDegValue+1 == degValue || degValue == maxDegValue-1) {
                // reset also sec
                newMin = 0.0;
            } else {
                m_ui->m_intValueEditor->setValue( degValue + 1 );
            }
            m_ui->m_floatValueEditor->setValue( newMin );
        }
    }
}

qreal DMInputHandler::calculateValue() const
{
    const bool isNegativeDeg = ( m_ui->m_intValueEditor->value() < 0 );

    const qreal deg = (qreal)(qAbs(m_ui->m_intValueEditor->value()));
    const qreal min = m_ui->m_floatValueEditor->value() / 60.0;

    qreal value = deg + min;

    if (isNegativeDeg) {
        value *= -1;
    }
    if (m_ui->m_sign->currentIndex() == NegativeSphereIndex) {
        value *= -1;
    }

    return value;
}


LatLonEditPrivate::LatLonEditPrivate()
    : m_value(0.0)
    , m_notation(GeoDataCoordinates::DMS)
    , m_inputHandler(new DMSInputHandler(this))
    , m_updating(false)
{}

LatLonEditPrivate::~LatLonEditPrivate()
{
    delete m_inputHandler;
}

void LatLonEditPrivate::init(QWidget* parent) { setupUi(parent); }

}


using namespace Marble;

LatLonEdit::LatLonEdit(QWidget *parent, Dimension dimension, GeoDataCoordinates::Notation notation)
    : QWidget( parent ), d(new LatLonEditPrivate())
{
    d->init(this);
    setDimension(dimension);
    setNotation(notation);

    connect(d->m_intValueEditor,   SIGNAL(valueChanged(int)),    this, SLOT(checkIntValueOverflow()));
    connect(d->m_uintValueEditor,  SIGNAL(valueChanged(int)),    this, SLOT(checkUIntValueOverflow()));
    connect(d->m_floatValueEditor, SIGNAL(valueChanged(double)), this, SLOT(checkFloatValueOverflow()));

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

GeoDataCoordinates::Notation LatLonEdit::notation() const
{
    return d->m_notation;
}

void LatLonEdit::onSignChanged()
{
    if( d->m_updating )
        return;

    // Only flip the value if it does not match the sign
    if (d->m_sign->currentIndex() == PositiveSphereIndex) {
        if (d->m_value < 0.0) {
            d->m_value *= -1;
        }
    } else {
        if (d->m_value > 0.0) {
            d->m_value *= -1;
        }
    }

    emit valueChanged( d->m_value );
}

void LatLonEdit::setDimension( Dimension dimension )
{
    if (d->m_dimension == dimension) {
        return;
    }

    d->m_dimension = dimension;

    d->m_updating = true;

    d->m_inputHandler->setupMinMax(dimension);

    // update sign widget content
    {
        d->m_sign->clear();

        switch (dimension) {
        case Longitude:
            d->m_sign->addItem( tr("E", "East, the direction" ) );
            d->m_sign->addItem( tr("W", "West, the direction" ) );
            break;
        case Latitude:
            d->m_sign->addItem( tr("N", "North, the direction" ) );
            d->m_sign->addItem( tr("S", "South, the direction" ) );
            break;
        }
    }

    d->m_updating = false;

    // reset value, old one is useless
    setValue( 0.0 );
}

void LatLonEdit::setNotation(GeoDataCoordinates::Notation notation)
{
    if( d->m_notation == notation ) {
        return;
    }

    delete d->m_inputHandler;

    switch (notation) {
    case GeoDataCoordinates::Decimal:
        d->m_inputHandler = new DecimalInputHandler(d);
        break;
    case GeoDataCoordinates::DMS:
        d->m_inputHandler = new DMSInputHandler(d);
        break;
    case GeoDataCoordinates::DM:
        d->m_inputHandler = new DMInputHandler(d);
        break;
    case GeoDataCoordinates::UTM:
        /** @todo implement */
        break;
    case GeoDataCoordinates::MGRS:
        /** @todo implement */
        break;
    }

    d->m_notation = notation;
    d->m_inputHandler->setupUi();
    d->m_inputHandler->setupMinMax(d->m_dimension);
    d->m_inputHandler->setValue(d->m_value);
}

void LatLonEdit::checkFloatValueOverflow()
{
    if( d->m_updating )
        return;

    d->m_updating = true;

    d->m_inputHandler->handleFloatEditChange();

    d->m_updating = false;

    recalculate();
}


void LatLonEdit::checkUIntValueOverflow()
{
    if( d->m_updating )
        return;

    d->m_updating = true;

    d->m_inputHandler->handleUIntEditChange();

    d->m_updating = false;

    recalculate();
}

void LatLonEdit::checkIntValueOverflow()
{
    if( d->m_updating )
        return;

    d->m_updating = true;

    d->m_inputHandler->handleIntEditChange();

    d->m_updating = false;

    recalculate();
}

void LatLonEdit::setValue( qreal value )
{
    // limit to allowed values
    const qreal maxValue = (d->m_dimension == Longitude) ? 180.0 : 90.0;

    if (value > maxValue) {
        value = maxValue;
    } else {
        const qreal minValue = -maxValue;
        if (value < minValue) {
            value = minValue;
        }
    }

    // no change?
    if( value == d->m_value ) {
        return;
    }

    d->m_value = value;

    // calculate sub values
    // calculation is done similar to GeoDataCoordinates::lonToString,
    // perhaps should be moved with similar methods into some utility class/namespace

    d->m_updating = true;

    d->m_inputHandler->setValue(value);

    const bool isNegative = (value < 0.0);
    d->m_sign->setCurrentIndex( isNegative ? NegativeSphereIndex : PositiveSphereIndex );

    d->m_updating = false;
}

void LatLonEdit::recalculate()
{
    d->m_value = d->m_inputHandler->calculateValue();

    emit valueChanged( d->m_value );
}


#include "LatLonEdit.moc"
