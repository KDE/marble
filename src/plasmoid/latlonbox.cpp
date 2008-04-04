/*
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of
** the License or (at your option) version 3 or any later version
** accepted by the membership of KDE e.V. (or its successor approved
** by the membership of KDE e.V.), which shall act as a proxy 
** defined in Section 14 of version 3 of the license.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//(C) 2008 Henry de Valence

#include <QHBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QObject>
#include <QDebug>

#include <cmath>

#include <marble/global.h>

#include "latlonbox.h"

LatLonBox::LatLonBox(QWidget *parent, Marble::Dimension dimension ) : QWidget( parent ),
	m_layout(0),
	m_degreesSpin(0),
	m_minutesSpin(0),
	m_secondsSpin(0),
	m_degreesLabel(0),
	m_minutesLabel(0),
	m_secondsLabel(0),
	m_comboBox(0)
{
	m_value = 0;
	m_dimension = dimension;

	m_layout = new QHBoxLayout;
	setLayout( m_layout );

	m_degreesSpin = new QSpinBox;
	m_comboBox = new QComboBox;
	if( m_dimension == Marble::Longitude ) {
		m_degreesSpin->setMinimum( -180 );
		m_degreesSpin->setMaximum( 180 );
		m_comboBox->addItem( tr("E", "East, the direction" ) );
		m_comboBox->addItem( tr("W", "West, the direction" ) );
	} else if( m_dimension == Marble::Latitude ) {
		m_degreesSpin->setMinimum( -90 );
		m_degreesSpin->setMaximum( 90 );
		m_comboBox->addItem( tr("N", "North, the direction" ) );
		m_comboBox->addItem( tr("S", "East, the direction" ) );
	} else {
		qDebug() << "Unrecognized dimension";
	}
	m_degreesSpin->show();
	m_comboBox->show();

	m_minutesSpin = new QSpinBox;
	//minimum value is -1 so we can
	//reduce the next one over
	m_minutesSpin->setMinimum( -1 );
	m_minutesSpin->setMaximum( 60 );
	m_minutesSpin->show();

	m_secondsSpin = new QSpinBox;
	m_secondsSpin->setMinimum( -1 );
	m_secondsSpin->setMaximum( 60 );
	m_secondsSpin->show();

	m_degreesLabel = new QLabel( QString::fromUtf8("\u00b0") );
	m_minutesLabel = new QLabel( QString("\'") );
	m_secondsLabel = new QLabel( QString("\"") );

	m_layout->addWidget( m_degreesSpin );
	m_layout->addWidget( m_degreesLabel );
	m_layout->addWidget( m_minutesSpin );
	m_layout->addWidget( m_minutesLabel );
	m_layout->addWidget( m_secondsSpin );
	m_layout->addWidget( m_secondsLabel );
	m_layout->addWidget( m_secondsLabel );
	m_layout->addWidget( m_comboBox );

	connect( m_secondsSpin, SIGNAL( valueChanged( int ) ),
	         this, SLOT( secondsOverflow( ) ) );

	connect( m_minutesSpin, SIGNAL( valueChanged( int ) ),
	         this, SLOT( minutesOverflow( ) ) );
	
	connect( m_degreesSpin, SIGNAL( valueChanged( int ) ),
	         this, SLOT( recalculate( ) ) );

	connect( m_comboBox, SIGNAL( currentIndexChanged( const QString & ) ),
	         this, SLOT( comboBoxChanged( const QString & ) ) );
}

void LatLonBox::checkComboBox()
{
	if( m_value < 0 ) {
		m_comboBox->setEnabled(true);
		if( m_dimension == Marble::Longitude && m_comboBox->currentText() == tr("N", "North, the direction" ) ) {
			m_comboBox->setCurrentIndex( m_comboBox->findText( tr("S", "South, the direction" ) ) );
		} 
		if( m_dimension == Marble::Latitude && m_comboBox->currentText() == tr("E", "East, the direction" ) ) {
			m_comboBox->setCurrentIndex( m_comboBox->findText( tr("W", "West, the direction" ) ) );
		} 
	} else if( m_value > 0 ) {
		m_comboBox->setEnabled(true);
		if( m_dimension == Marble::Longitude && m_comboBox->currentText() == tr("S", "South, the direction" ) ) {
			m_comboBox->setCurrentIndex( m_comboBox->findText( tr("N", "North, the direction" ) ) );
		} 
		if( m_dimension == Marble::Latitude && m_comboBox->currentText() == tr("W", "West, the direction" ) ) {
			m_comboBox->setCurrentIndex( m_comboBox->findText( tr("E", "East, the direction" ) ) );
		} 
	} else {
		//m_value is zero, so long/lat do not apply
		m_comboBox->setEnabled(false);
	}
}

void LatLonBox::comboBoxChanged( const QString &text )
{
	if( ( text == tr("N", "North, the direction" ) || text == tr("E", "East, the direction" ) ) &&
	      m_value < 0 ) {
		m_value -= m_value * 2;
		reverseRecalculate();
	} else if( ( text == tr("S", "South, the direction" ) || text == tr("W", "West, the direction" ) ) &&
	      m_value > 0 ) {
		m_value -= m_value * 2;
		reverseRecalculate();
	}
}

void LatLonBox::setDimension( Marble::Dimension dimension )
{
	//don't do anything
	if( m_dimension == dimension ) {
		return;
	}

	if( m_dimension == Marble::Longitude ) {
		m_comboBox->removeItem( m_comboBox->findText( tr("W", "West, the direction" ) ) );
		m_comboBox->removeItem( m_comboBox->findText( tr("E", "East, the direction" ) ) );

		m_comboBox->addItem( tr("N", "North, the direction" ) );
		m_comboBox->addItem( tr("S", "South, the direction" ) );
	} else if( m_dimension == Marble::Latitude ) {
		m_comboBox->removeItem( m_comboBox->findText( tr("N", "North, the direction" ) ) );
		m_comboBox->removeItem( m_comboBox->findText( tr("S", "South, the direction" ) ) );

		m_comboBox->addItem( tr("E", "East, the direction" ) );
		m_comboBox->addItem( tr("W", "West, the direction" ) );
	} else {
		//unknown dimension
		qDebug() << "unknown dimension, not changing";
	}
	m_dimension = dimension;

}

void LatLonBox::secondsOverflow()
{
	if( m_secondsSpin->value() == 60 ) {
		m_secondsSpin->setValue( 0 );
		m_minutesSpin->setValue( m_minutesSpin->value() + 1 );
		recalculate();
		return;
	} else if( m_secondsSpin->value() == -1 ) {
		m_secondsSpin->setValue( 59 );
		m_minutesSpin->setValue( m_minutesSpin->value() - 1 );
		recalculate();
		return;
	}
	recalculate();
}
	
void LatLonBox::minutesOverflow()
{
	if( m_minutesSpin->value() == 60 ) {
		m_minutesSpin->setValue( 0 );
		m_degreesSpin->setValue( m_degreesSpin->value() + 1 );
		recalculate();
		return;
	} else if( m_minutesSpin->value() == -1 ) {
		m_minutesSpin->setValue( 59 );
		m_degreesSpin->setValue( m_degreesSpin->value() - 1 );
		recalculate();
		return;
	}
	recalculate();
}

void LatLonBox::setValue( double newvalue )
{
	m_value = newvalue;
	reverseRecalculate();
	qDebug() << "valueChanged: now " << m_value;
	emit valueChanged( m_value );
}

void LatLonBox::recalculate()
{
	double newvalue = m_degreesSpin->value();
	double minsfract = m_minutesSpin->value();
	double secsfract = m_secondsSpin->value();

	minsfract = minsfract / 60;
	secsfract = secsfract / 3600;

	qDebug() << "newvalue = " << newvalue;
	qDebug() << "minsfract = " << minsfract;
	qDebug() << "secsfract = " << secsfract;

	//we need two because if the degrees is neg,
	//the mins/secs *subtract* from the value
	if( m_degreesSpin->value() >= 0 ) {
		newvalue += minsfract;
		newvalue += secsfract;
	} else {
		newvalue -= minsfract;
		newvalue -= secsfract;
	}

	m_value = newvalue;

	//you can't have 180*59'59" , only 180.
	//but lat. & lon. have different maximums, so we check
	if( m_degreesSpin->value() == m_degreesSpin->maximum() || 
	    m_degreesSpin->value() == m_degreesSpin->minimum() ) {
		m_minutesSpin->setValue( 0 );
		m_secondsSpin->setValue( 0 );
	} 

	if( m_degreesSpin->value() < m_degreesSpin->minimum() ) {
		m_degreesSpin->setValue( m_degreesSpin->minimum() );
		m_minutesSpin->setValue( 0 );
		m_secondsSpin->setValue( 0 );
	} 

	if( m_degreesSpin->value() > m_degreesSpin->maximum() ) {
		m_degreesSpin->setValue( m_degreesSpin->maximum() );
		m_minutesSpin->setValue( 0 );
		m_secondsSpin->setValue( 0 );
	} 
	qDebug() << "valueChanged: now " << m_value;

	checkComboBox();

	//put this last so combobox &c will be correct
	//for stuff that needs it
	emit valueChanged( m_value );
			
}


void LatLonBox::reverseRecalculate()
{
	int degreesvalue = 0;	
	int minutesvalue = 0;
	int secondsvalue = 0;

	//degreesvalue is the whole degree part
	if( m_value >= 0 ) {
		degreesvalue = floor( m_value );
	} else {
		degreesvalue = ceil( m_value );
	}

	//minutesremainder is the fraction of a degree that
	//is left over
	double minutesremainder = m_value - degreesvalue;
	//multipy the fraction of a degree by 60 to
	//turn it into minutes
	minutesremainder = minutesremainder * 60;
	//minutesvalue is the whole minutes part
	if( m_value >= 0 ) {
		minutesvalue = floor( minutesremainder );
	} else {
		minutesvalue = ceil( minutesremainder );
	}

	//secondsremainder is the fraction of a minute
	//that is left over
	double secondsremainder = minutesremainder - minutesvalue;
	//multiply the fraction of a minute by 60 to
	//turn it into seconds
	secondsvalue = secondsremainder * 60;

	//now we have all the values, we 
	//put them on the boxes.
	m_degreesSpin->setValue( degreesvalue );
	m_minutesSpin->setValue( minutesvalue );
	m_secondsSpin->setValue( secondsvalue );

	checkComboBox();
}




