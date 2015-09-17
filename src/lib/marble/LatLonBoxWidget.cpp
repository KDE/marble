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

class Q_DECL_HIDDEN LatLonBoxWidget::Private
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
    // used for updating tiles count in DownloadRegionDialog
    connect( d->m_ui.northSpinBox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
    connect( d->m_ui.southSpinBox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
    connect( d->m_ui.eastSpinBox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
    connect( d->m_ui.westSpinBox, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
    // used for adjusting single step values
    connect( d->m_ui.northSpinBox, SIGNAL(valueChanged(double)), SLOT(updateLatSingleStep()));
    connect( d->m_ui.southSpinBox, SIGNAL(valueChanged(double)), SLOT(updateLatSingleStep()));
    connect( d->m_ui.eastSpinBox, SIGNAL(valueChanged(double)), SLOT(updateLonSingleStep()));
    connect( d->m_ui.westSpinBox, SIGNAL(valueChanged(double)), SLOT(updateLonSingleStep()));
}

LatLonBoxWidget::~LatLonBoxWidget()
{
    delete d;
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

void LatLonBoxWidget::updateLatSingleStep()
{
    double const singleStep =
        qAbs( d->m_ui.northSpinBox->value() - d->m_ui.southSpinBox->value() ) / 10.0;
    d->m_ui.northSpinBox->setSingleStep( singleStep );
    d->m_ui.southSpinBox->setSingleStep( singleStep );
}

void LatLonBoxWidget::updateLonSingleStep()
{
    double const singleStep =
        qAbs( d->m_ui.eastSpinBox->value() - d->m_ui.westSpinBox->value() ) / 10.0;
    d->m_ui.eastSpinBox->setSingleStep( singleStep );
    d->m_ui.westSpinBox->setSingleStep( singleStep );
}

}

#include "moc_LatLonBoxWidget.cpp"
