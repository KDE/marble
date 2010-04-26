// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#include "LatLonBoxWidget.h"

#include "GeoDataLatLonBox.h"
#include "ui_LatLonBoxWidget.h"

namespace Marble
{

class LatLonBoxWidget::Private
{
public:
    explicit Private( QWidget * const parent );
    Ui::LatLonBoxWidget m_ui;
};

LatLonBoxWidget::Private::Private( QWidget * const parent )
{
    m_ui.setupUi( parent );
}


LatLonBoxWidget::LatLonBoxWidget( QWidget * const parent, Qt::WindowFlags const f )
    : QWidget( parent, f ),
      d( new Private( this ))
{
    connect( d->m_ui.northSpinBox, SIGNAL( valueChanged( double )), SIGNAL( valueChanged() ));
    connect( d->m_ui.southSpinBox, SIGNAL( valueChanged( double )), SIGNAL( valueChanged() ));
    connect( d->m_ui.eastSpinBox, SIGNAL( valueChanged( double )), SIGNAL( valueChanged() ));
    connect( d->m_ui.westSpinBox, SIGNAL( valueChanged( double )), SIGNAL( valueChanged() ));
}

GeoDataLatLonBox LatLonBoxWidget::latLonBox() const
{
    return GeoDataLatLonBox( d->m_ui.northSpinBox->value(), d->m_ui.southSpinBox->value(),
                             d->m_ui.eastSpinBox->value(), d->m_ui.westSpinBox->value(),
                             GeoDataCoordinates::Degree );
}

void LatLonBoxWidget::setLatLonBox( GeoDataLatLonBox const & latLonBox )
{
    d->m_ui.northSpinBox->setValue( latLonBox.north( GeoDataCoordinates::Degree ));
    d->m_ui.southSpinBox->setValue( latLonBox.south( GeoDataCoordinates::Degree ));
    d->m_ui.eastSpinBox->setValue( latLonBox.east( GeoDataCoordinates::Degree ));
    d->m_ui.westSpinBox->setValue( latLonBox.west( GeoDataCoordinates::Degree ));
}

}

#include "LatLonBoxWidget.moc"
