// SPDX-License-Identifier: LGPL-2.1-or-later

#include "LatLonBoxWidget.h"

#include "GeoDataLatLonBox.h"
#include "ui_LatLonBoxWidget.h"

namespace Marble
{

class Q_DECL_HIDDEN LatLonBoxWidget::Private
{
public:
    explicit Private(QWidget *const parent);
    Ui::LatLonBoxWidget m_ui;
};

LatLonBoxWidget::Private::Private(QWidget *const parent)
{
    m_ui.setupUi(parent);
}

LatLonBoxWidget::LatLonBoxWidget(QWidget *const parent, Qt::WindowFlags const f)
    : QWidget(parent, f)
    , d(new Private(this))
{
    // used for updating tiles count in DownloadRegionDialog
    connect(d->m_ui.northSpinBox, &QDoubleSpinBox::valueChanged, this, &LatLonBoxWidget::valueChanged);
    connect(d->m_ui.southSpinBox, &QDoubleSpinBox::valueChanged, this, &LatLonBoxWidget::valueChanged);
    connect(d->m_ui.eastSpinBox, &QDoubleSpinBox::valueChanged, this, &LatLonBoxWidget::valueChanged);
    connect(d->m_ui.westSpinBox, &QDoubleSpinBox::valueChanged, this, &LatLonBoxWidget::valueChanged);
    // used for adjusting single step values
    connect(d->m_ui.northSpinBox, &QDoubleSpinBox::valueChanged, this, &LatLonBoxWidget::updateLatSingleStep);
    connect(d->m_ui.southSpinBox, &QDoubleSpinBox::valueChanged, this, &LatLonBoxWidget::updateLatSingleStep);
    connect(d->m_ui.eastSpinBox, &QDoubleSpinBox::valueChanged, this, &LatLonBoxWidget::updateLonSingleStep);
    connect(d->m_ui.westSpinBox, &QDoubleSpinBox::valueChanged, this, &LatLonBoxWidget::updateLonSingleStep);
}

LatLonBoxWidget::~LatLonBoxWidget()
{
    delete d;
}

GeoDataLatLonBox LatLonBoxWidget::latLonBox() const
{
    return {d->m_ui.northSpinBox->value(),
            d->m_ui.southSpinBox->value(),
            d->m_ui.eastSpinBox->value(),
            d->m_ui.westSpinBox->value(),
            GeoDataCoordinates::Degree};
}

void LatLonBoxWidget::setLatLonBox(GeoDataLatLonBox const &latLonBox)
{
    d->m_ui.northSpinBox->setValue(latLonBox.north(GeoDataCoordinates::Degree));
    d->m_ui.southSpinBox->setValue(latLonBox.south(GeoDataCoordinates::Degree));
    d->m_ui.eastSpinBox->setValue(latLonBox.east(GeoDataCoordinates::Degree));
    d->m_ui.westSpinBox->setValue(latLonBox.west(GeoDataCoordinates::Degree));
}

void LatLonBoxWidget::updateLatSingleStep()
{
    double const singleStep = qAbs(d->m_ui.northSpinBox->value() - d->m_ui.southSpinBox->value()) / 10.0;
    d->m_ui.northSpinBox->setSingleStep(singleStep);
    d->m_ui.southSpinBox->setSingleStep(singleStep);
}

void LatLonBoxWidget::updateLonSingleStep()
{
    double const singleStep = qAbs(d->m_ui.eastSpinBox->value() - d->m_ui.westSpinBox->value()) / 10.0;
    d->m_ui.eastSpinBox->setSingleStep(singleStep);
    d->m_ui.westSpinBox->setSingleStep(singleStep);
}

}

#include "moc_LatLonBoxWidget.cpp"
