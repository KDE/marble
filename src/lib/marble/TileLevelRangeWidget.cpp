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

class Q_DECL_HIDDEN TileLevelRangeWidget::Private
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
    connect( d->m_ui.topSpinBox, SIGNAL(valueChanged(int)), SIGNAL(topLevelChanged(int)));
    connect( d->m_ui.bottomSpinBox, SIGNAL(valueChanged(int)),
             SIGNAL(bottomLevelChanged(int)));

    connect( d->m_ui.topSpinBox, SIGNAL(valueChanged(int)), SLOT(setMinimumBottomLevel(int)));
    connect( d->m_ui.bottomSpinBox, SIGNAL(valueChanged(int)), SLOT(setMaximumTopLevel(int)));
}

TileLevelRangeWidget::~TileLevelRangeWidget()
{
    delete d;
}

QSize TileLevelRangeWidget::sizeHint() const
{
    return size();
}

void TileLevelRangeWidget::setAllowedLevelRange( int const minimumLevel, int const maximumLevel )
{
    d->m_ui.topSpinBox->setRange( minimumLevel, qMin( d->m_ui.bottomSpinBox->value(),
                                                      maximumLevel ));
    d->m_ui.bottomSpinBox->setRange( qMax( d->m_ui.topSpinBox->value(), minimumLevel ),
                                     maximumLevel );
}

void TileLevelRangeWidget::setDefaultLevel( int const level )
{
    d->m_ui.topSpinBox->setValue( level );
    d->m_ui.bottomSpinBox->setValue( level );
}

int TileLevelRangeWidget::bottomLevel() const
{
    return d->m_ui.bottomSpinBox->value();
}

int TileLevelRangeWidget::topLevel() const
{
    return d->m_ui.topSpinBox->value();
}

void TileLevelRangeWidget::setMaximumTopLevel( int const level )
{
    d->m_ui.topSpinBox->setMaximum( level );
}

void TileLevelRangeWidget::setMinimumBottomLevel( int const level )
{
    d->m_ui.bottomSpinBox->setMinimum( level );
}

}

#include "moc_TileLevelRangeWidget.cpp"
