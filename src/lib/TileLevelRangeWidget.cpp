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

#include "TileLevelRangeWidget.h"

#include "ui_TileLevelRangeWidget.h"

namespace Marble
{

class TileLevelRangeWidget::Private
{
public:
    explicit Private( QWidget * const parent );
    Ui::TileLevelRangeWidget m_ui;
};

TileLevelRangeWidget::Private::Private( QWidget * const parent )
{
    m_ui.setupUi( parent );
}

TileLevelRangeWidget::TileLevelRangeWidget( QWidget * const parent, Qt::WindowFlags const f )
    : QWidget( parent, f ),
      d( new Private( this ))
{
    // FIXME: rename min -> topLevel, max -> bottomLevel
    connect( d->m_ui.minSpinBox, SIGNAL( valueChanged( int )), SIGNAL( topLevelChanged( int )));
    connect( d->m_ui.maxSpinBox, SIGNAL( valueChanged( int )), SIGNAL( bottomLevelChanged( int )));

    connect( d->m_ui.minSpinBox, SIGNAL( valueChanged( int )), SLOT( setMinimumBottomLevel( int )));
    connect( d->m_ui.maxSpinBox, SIGNAL( valueChanged( int )), SLOT( setMaximumTopLevel( int )));
}

TileLevelRangeWidget::~TileLevelRangeWidget()
{
    delete d;
}

void TileLevelRangeWidget::setAllowedTileLevelRange( int const minimumTileLevel,
                                                     int const maximumTileLevel )
{
    d->m_ui.minSpinBox->setRange( minimumTileLevel, qMin( d->m_ui.maxSpinBox->value(),
                                                          maximumTileLevel ));
    d->m_ui.maxSpinBox->setRange( qMax( d->m_ui.minSpinBox->value(), minimumTileLevel ),
                                  maximumTileLevel );
}

void TileLevelRangeWidget::setDefaultTileLevel( int const tileLevel )
{
    d->m_ui.minSpinBox->setValue( tileLevel );
    d->m_ui.maxSpinBox->setValue( tileLevel );
}

int TileLevelRangeWidget::bottomLevel() const
{
    return d->m_ui.maxSpinBox->value();
}

int TileLevelRangeWidget::topLevel() const
{
    return d->m_ui.minSpinBox->value();
}

void TileLevelRangeWidget::setMaximumTopLevel( int const level )
{
    d->m_ui.minSpinBox->setMaximum( level );
}

void TileLevelRangeWidget::setMinimumBottomLevel( int const level )
{
    d->m_ui.maxSpinBox->setMinimum( level );
}

}

#include "TileLevelRangeWidget.moc"
