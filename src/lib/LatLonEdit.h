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

#ifndef LATLONEDIT_H
#define LATLONEDIT_H

#include <QtGui/QWidget>
////QWidgetPlugin is used for Qt Designer
#include <QtCore/QtPlugin>
#include <QtDesigner/QDesignerExportWidget>

class QSpinBox;
class QComboBox;
class QHBoxLayout;
class QLabel;

#include "global.h"
#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT /* QDESIGNER_WIDGET_EXPORT*/ LatLonEdit : public QWidget
{
	Q_OBJECT
	//Q_PROPERTY( double m_value READ value WRITE setValue )
	//Q_PROPERTY( int m_dimension READ dimension WRITE setDimension )
public:
	explicit LatLonEdit(QWidget *parent = 0, Marble::Dimension dimension = Marble::Longitude );
	double value();
	Marble::Dimension dimension();
public Q_SLOTS:
	void setValue(double newvalue);
	void setDimension( Marble::Dimension dimension );
Q_SIGNALS:
	void dimensionChanged( Marble::Dimension dimension );
	void valueChanged( double value );
private Q_SLOTS:
	// changes value based on combobox
	void comboBoxChanged( int index );
	// recalculates m_value based on spinboxes
	void recalculate();
	// recalculates spinboxes based on m_value
	void reverseRecalculate();
	// changes combobox based on value
	void checkComboBox();
	void secondsOverflow();
	void minutesOverflow();
private:
	QHBoxLayout *m_layout;

	int m_dimension;

	QComboBox *m_comboBox;

	QSpinBox *m_degreesSpin;
	QSpinBox *m_minutesSpin;
	QSpinBox *m_secondsSpin;

	QLabel *m_degreesLabel;
	QLabel *m_minutesLabel;
	QLabel *m_secondsLabel;

	double m_value;
};

}

#endif
