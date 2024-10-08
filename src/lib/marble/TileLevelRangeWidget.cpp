// SPDX-License-Identifier: LGPL-2.1-or-later

#include "TileLevelRangeWidget.h"

#include "ui_TileLevelRangeWidget.h"

namespace Marble
{

class Q_DECL_HIDDEN TileLevelRangeWidget::Private
{
public:
    explicit Private(QWidget *const parent);
    Ui::TileLevelRangeWidget m_ui;
};

TileLevelRangeWidget::Private::Private(QWidget *const parent)
{
    m_ui.setupUi(parent);
}

TileLevelRangeWidget::TileLevelRangeWidget(QWidget *const parent, Qt::WindowFlags const f)
    : QWidget(parent, f)
    , d(new Private(this))
{
    connect(d->m_ui.topSpinBox, &QSpinBox::valueChanged, this, &TileLevelRangeWidget::topLevelChanged);
    connect(d->m_ui.bottomSpinBox, &QSpinBox::valueChanged, this, &TileLevelRangeWidget::bottomLevelChanged);

    connect(d->m_ui.topSpinBox, &QSpinBox::valueChanged, this, &TileLevelRangeWidget::setMinimumBottomLevel);
    connect(d->m_ui.bottomSpinBox, &QSpinBox::valueChanged, this, &TileLevelRangeWidget::setMaximumTopLevel);
}

TileLevelRangeWidget::~TileLevelRangeWidget()
{
    delete d;
}

QSize TileLevelRangeWidget::sizeHint() const
{
    return size();
}

void TileLevelRangeWidget::setAllowedLevelRange(int const minimumLevel, int const maximumLevel)
{
    d->m_ui.topSpinBox->setRange(minimumLevel, qMin(d->m_ui.bottomSpinBox->value(), maximumLevel));
    d->m_ui.bottomSpinBox->setRange(qMax(d->m_ui.topSpinBox->value(), minimumLevel), maximumLevel);
}

void TileLevelRangeWidget::setDefaultLevel(int const level)
{
    d->m_ui.topSpinBox->setValue(level);
    d->m_ui.bottomSpinBox->setValue(level);
}

int TileLevelRangeWidget::bottomLevel() const
{
    return d->m_ui.bottomSpinBox->value();
}

int TileLevelRangeWidget::topLevel() const
{
    return d->m_ui.topSpinBox->value();
}

void TileLevelRangeWidget::setMaximumTopLevel(int const level)
{
    d->m_ui.topSpinBox->setMaximum(level);
}

void TileLevelRangeWidget::setMinimumBottomLevel(int const level)
{
    d->m_ui.bottomSpinBox->setMinimum(level);
}

}

#include "moc_TileLevelRangeWidget.cpp"
