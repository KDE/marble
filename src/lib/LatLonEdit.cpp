//Copyright 2008 Henry de Valence <hdevalence@gmail.com>
/*
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public License as
** published by the Free Software Foundation; either version 2 of
** the License or (at your option) version 3 or any later version
** accepted by the membership of KDE e.V. (or its successor approved
** by the membership of KDE e.V.), which shall act as a proxy 
** defined in Section 14 of version 3 of the license.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "LatLonEdit.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <QtCore/QDebug>

#include <cmath>

#include "global.h"

using namespace Marble;

namespace Marble {

class LatLonEditPrivate
{
public:
	QHBoxLayout *m_layout;

	int m_dimension;

	QComboBox *m_comboBox;

	QSpinBox *m_degreesSpin;
	QSpinBox *m_minutesSpin;
	QSpinBox *m_secondsSpin;

	QLabel *m_degreesLabel;
	QLabel *m_minutesLabel;
	QLabel *m_secondsLabel;

	qreal m_value;

	LatLonEditPrivate() :
		m_layout(0),
		m_comboBox(0),
		m_degreesSpin(0),
		m_minutesSpin(0),
		m_secondsSpin(0),
		m_degreesLabel(0),
		m_minutesLabel(0),
		m_secondsLabel(0) { }
};

}

LatLonEdit::LatLonEdit(QWidget *parent, Marble::Dimension dimension ) : QWidget( parent ),
	d(new LatLonEditPrivate)
{
	d->m_value = 0;
	d->m_dimension = static_cast<int>( dimension );

	d->m_layout = new QHBoxLayout;
	setLayout( d->m_layout );

	d->m_degreesSpin = new QSpinBox;
	d->m_comboBox = new QComboBox;
	//in d->m_combobox, the +ve is 0 and the -ve is 1 in the index
	if( d->m_dimension == Marble::Longitude ) {
		d->m_degreesSpin->setMinimum( -180 );
		d->m_degreesSpin->setMaximum( 180 );
		d->m_comboBox->addItem( tr("E", "East, the direction" ) );
		d->m_comboBox->addItem( tr("W", "West, the direction" ) );
	} else if( d->m_dimension == Marble::Latitude ) {
		d->m_degreesSpin->setMinimum( -90 );
		d->m_degreesSpin->setMaximum( 90 );
		d->m_comboBox->addItem( tr("N", "North, the direction" ) );
		d->m_comboBox->addItem( tr("S", "South, the direction" ) );
	} else {
		qDebug() << "Unrecognized dimension";
	}
	d->m_degreesSpin->show();
	//it's grayed out when it's 0, and that is the default
	d->m_comboBox->setEnabled(false);
	d->m_comboBox->show();

	d->m_minutesSpin = new QSpinBox;
	//minimum value is -1 so we can
	//reduce the next one over
	d->m_minutesSpin->setMinimum( -1 );
	d->m_minutesSpin->setMaximum( 60 );
	d->m_minutesSpin->show();

	d->m_secondsSpin = new QSpinBox;
	d->m_secondsSpin->setMinimum( -1 );
	d->m_secondsSpin->setMaximum( 60 );
	d->m_secondsSpin->show();

	d->m_degreesLabel = new QLabel( QString::fromUtf8("\u00b0") );
	d->m_minutesLabel = new QLabel( QString("\'") );
	d->m_secondsLabel = new QLabel( QString("\"") );

	d->m_layout->addWidget( d->m_degreesSpin );
	d->m_layout->addWidget( d->m_degreesLabel );
	d->m_layout->addWidget( d->m_minutesSpin );
	d->m_layout->addWidget( d->m_minutesLabel );
	d->m_layout->addWidget( d->m_secondsSpin );
	d->m_layout->addWidget( d->m_secondsLabel );
	d->m_layout->addWidget( d->m_secondsLabel );
	d->m_layout->addWidget( d->m_comboBox );

	connect( d->m_secondsSpin, SIGNAL( valueChanged( int ) ),
	         this, SLOT( secondsOverflow( ) ) );

	connect( d->m_minutesSpin, SIGNAL( valueChanged( int ) ),
	         this, SLOT( minutesOverflow( ) ) );
	
	connect( d->m_degreesSpin, SIGNAL( valueChanged( int ) ),
	         this, SLOT( recalculate( ) ) );

	connect( d->m_comboBox, SIGNAL( currentIndexChanged( int ) ),
	         this, SLOT( comboBoxChanged( int ) ) );
}

LatLonEdit::~LatLonEdit()
{
	delete d;
}

qreal LatLonEdit::value()
{
	return d->m_value;
}

void LatLonEdit::checkComboBox()
{
	if( d->m_value < 0 ) {
		//in case it was disabled by being set to 0
		d->m_comboBox->setEnabled(true);
		if( d->m_comboBox->currentIndex() == 0 ) {
			d->m_comboBox->setCurrentIndex( 1 );
		} 
	} else if( d->m_value > 0 ) {
		d->m_comboBox->setEnabled(true);
		if( d->m_comboBox->currentIndex() == 1 ) {
			d->m_comboBox->setCurrentIndex( 0 );
		} 
	} else {
		//d->m_value is zero, so long/lat do not apply
		d->m_comboBox->setEnabled(false);
	}
}

void LatLonEdit::comboBoxChanged( int index )
{
	if( index == 0 && d->m_value < 0 ) {
		d->m_value -= d->m_value * 2;
		reverseRecalculate();
	} else if( index == 1 && d->m_value > 0 ) {
		d->m_value -= d->m_value * 2;
		reverseRecalculate();
	}
}

void LatLonEdit::setDimension( Marble::Dimension dimension )
{
	//don't do anything
	if( d->m_dimension == dimension ) {
		return;
	}

	//in d->m_combobox, the +ve is 0 and the -ve is 1 in the index
	if( d->m_dimension == Marble::Longitude ) {
		d->m_comboBox->removeItem( 0 );
		d->m_comboBox->removeItem( 1 );

		d->m_comboBox->addItem( tr("N", "North, the direction" ) );
		d->m_comboBox->addItem( tr("S", "South, the direction" ) );
	} else if( d->m_dimension == Marble::Latitude ) {
		d->m_comboBox->removeItem( 0 );
		d->m_comboBox->removeItem( 1 );

		d->m_comboBox->addItem( tr("E", "East, the direction" ) );
		d->m_comboBox->addItem( tr("W", "West, the direction" ) );
	} 
	d->m_dimension = dimension;

}

void LatLonEdit::secondsOverflow()
{
	if( d->m_secondsSpin->value() == 60 ) {
		d->m_secondsSpin->setValue( 0 );
		d->m_minutesSpin->setValue( d->m_minutesSpin->value() + 1 );
		recalculate();
		return;
	} else if( d->m_secondsSpin->value() == -1 ) {
		d->m_secondsSpin->setValue( 59 );
		d->m_minutesSpin->setValue( d->m_minutesSpin->value() - 1 );
		recalculate();
		return;
	}
	recalculate();
}
	
void LatLonEdit::minutesOverflow()
{
	if( d->m_minutesSpin->value() == 60 ) {
		d->m_minutesSpin->setValue( 0 );
		d->m_degreesSpin->setValue( d->m_degreesSpin->value() + 1 );
		recalculate();
		return;
	} else if( d->m_minutesSpin->value() == -1 ) {
		d->m_minutesSpin->setValue( 59 );
		d->m_degreesSpin->setValue( d->m_degreesSpin->value() - 1 );
		recalculate();
		return;
	}
	recalculate();
}

void LatLonEdit::setValue( qreal newvalue )
{
	d->m_value = newvalue;
	reverseRecalculate();
	//qDebug() << "valueChanged: now " << d->m_value;
	emit valueChanged( d->m_value );
}

void LatLonEdit::recalculate()
{
	qreal newvalue = d->m_degreesSpin->value();
	qreal minsfract = d->m_minutesSpin->value();
	qreal secsfract = d->m_secondsSpin->value();

	minsfract = minsfract / 60;
	secsfract = secsfract / 3600;

        /*
	qDebug() << "newvalue = " << newvalue;
	qDebug() << "minsfract = " << minsfract;
	qDebug() << "secsfract = " << secsfract;
        */

	//we need two because if the degrees is neg,
	//the mins/secs *subtract* from the value
	if( d->m_degreesSpin->value() >= 0 ) {
		newvalue += minsfract;
		newvalue += secsfract;
	} else {
		newvalue -= minsfract;
		newvalue -= secsfract;
	}

	d->m_value = newvalue;

	//you can't have 180*59'59" , only 180.
	//but lat. & lon. have different maximums, so we check
	if( d->m_degreesSpin->value() == d->m_degreesSpin->maximum() || 
	    d->m_degreesSpin->value() == d->m_degreesSpin->minimum() ) {
		d->m_minutesSpin->setValue( 0 );
		d->m_secondsSpin->setValue( 0 );
	} 

	if( d->m_degreesSpin->value() < d->m_degreesSpin->minimum() ) {
		d->m_degreesSpin->setValue( d->m_degreesSpin->minimum() );
		d->m_minutesSpin->setValue( 0 );
		d->m_secondsSpin->setValue( 0 );
	} 

	if( d->m_degreesSpin->value() > d->m_degreesSpin->maximum() ) {
		d->m_degreesSpin->setValue( d->m_degreesSpin->maximum() );
		d->m_minutesSpin->setValue( 0 );
		d->m_secondsSpin->setValue( 0 );
	} 
	//qDebug() << "valueChanged: now " << d->m_value;

	checkComboBox();

	//put this last so combobox &c will be correct
	//for stuff that needs it
	emit valueChanged( d->m_value );
			
}


void LatLonEdit::reverseRecalculate()
{
	int degreesvalue = 0;	
	int minutesvalue = 0;
	int secondsvalue = 0;

	//degreesvalue is the whole degree part
	if( d->m_value >= 0 ) {
		degreesvalue = floor( d->m_value );
	} else {
		degreesvalue = ceil( d->m_value );
	}

	//minutesremainder is the fraction of a degree that
	//is left over
	qreal minutesremainder = d->m_value - degreesvalue;
	//multipy the fraction of a degree by 60 to
	//turn it into minutes
	minutesremainder = minutesremainder * 60;
	//minutesvalue is the whole minutes part
	if( d->m_value >= 0 ) {
		minutesvalue = floor( minutesremainder );
	} else {
		minutesvalue = ceil( minutesremainder );
	}

	//secondsremainder is the fraction of a minute
	//that is left over
	qreal secondsremainder = minutesremainder - minutesvalue;
	//multiply the fraction of a minute by 60 to
	//turn it into seconds
	secondsvalue = secondsremainder * 60;

	//now we have all the values, we 
	//put them on the boxes.
	d->m_degreesSpin->setValue( degreesvalue );
	d->m_minutesSpin->setValue( minutesvalue );
	d->m_secondsSpin->setValue( secondsvalue );

	checkComboBox();
}

#include "LatLonEdit.moc"



