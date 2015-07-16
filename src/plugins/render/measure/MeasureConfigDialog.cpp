//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>

#include <QPushButton>
#include "MeasureConfigDialog.h"

namespace Marble {

MeasureConfigDialog::MeasureConfigDialog(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::MeasureConfigDialog)
{
    ui->setupUi(this);

    QPushButton *apply = ui->m_buttonBox->button(QDialogButtonBox::Apply);
    connect(apply, SIGNAL(clicked()), this, SIGNAL(applied()));
    connect(ui->m_modeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateTabs()));
    connect(ui->m_modeCombo, SIGNAL(currentIndexChanged(int)),
            ui->tabWidget, SLOT(setCurrentIndex(int)));
}

MeasureConfigDialog::~MeasureConfigDialog()
{
    delete ui;
}

bool MeasureConfigDialog::showBearingLabel() const
{
    return ui->m_showBearingLabelsCheckBox->isChecked();
}

bool MeasureConfigDialog::showBearingLabelChange() const
{
    return ui->m_showBearingLabelChangeCheckBox->isChecked();
}

bool MeasureConfigDialog::showDistanceLabels() const
{
    return ui->m_showDistanceLabelsCheckBox->isChecked();
}

bool MeasureConfigDialog::showRadius() const
{
    return ui->m_showRadiusLabelsCheckBox->isChecked();
}

bool MeasureConfigDialog::showPerimeter() const
{
    return ui->m_showPerimeter->isChecked();
}

bool MeasureConfigDialog::showCircumference() const
{
    return ui->m_showCircumference->isChecked();
}

bool MeasureConfigDialog::showPolygonArea() const
{
    return ui->m_showPolygonAreaLabelChangeCheckBox->isChecked();
}

bool MeasureConfigDialog::showCircularArea() const
{
    return ui->m_showCircleAreaLabelChangeCheckBox->isChecked();
}

MeasureToolPlugin::PaintMode MeasureConfigDialog::paintMode() const
{
    return (MeasureToolPlugin::PaintMode)ui->m_modeCombo->currentIndex();
}

void MeasureConfigDialog::setShowBearingLabel(bool show)
{
    ui->m_showBearingLabelsCheckBox->setChecked(show);
}

void MeasureConfigDialog::setShowBearingLabelChange(bool show)
{
    ui->m_showBearingLabelChangeCheckBox->setChecked(show);
}

void MeasureConfigDialog::setShowDistanceLabels(bool show)
{
    ui->m_showDistanceLabelsCheckBox->setChecked(show);
}

void MeasureConfigDialog::setShowRadius(bool show)
{
    ui->m_showRadiusLabelsCheckBox->setChecked(show);
}

void MeasureConfigDialog::setShowPerimeter(bool show)
{
    ui->m_showPerimeter->setChecked(show);
}

void MeasureConfigDialog::setShowCircumference(bool show)
{
    ui->m_showCircumference->setChecked(show);
}

void MeasureConfigDialog::setShowPolygonArea(bool show)
{
    ui->m_showPolygonAreaLabelChangeCheckBox->setChecked(show);
}

void MeasureConfigDialog::setShowCircularArea(bool show)
{
    ui->m_showCircleAreaLabelChangeCheckBox->setChecked(show);
}

void MeasureConfigDialog::setPaintMode(MeasureToolPlugin::PaintMode mode)
{
    ui->m_modeCombo->setCurrentIndex((int)mode);
    updateTabs();
}

void MeasureConfigDialog::updateTabs()
{
    switch (paintMode()) {
    case MeasureToolPlugin::Polygon:
        ui->tabWidget->setTabEnabled(0, true);
        ui->tabWidget->setTabEnabled(1, false);
        break;
    case MeasureToolPlugin::Circular:
        ui->tabWidget->setTabEnabled(0, false);
        ui->tabWidget->setTabEnabled(1, true);
        break;
    }
}

} // namespace Marble

#include "moc_MeasureConfigDialog.cpp"
