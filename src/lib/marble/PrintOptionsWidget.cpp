// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "PrintOptionsWidget.h"

namespace Marble
{

PrintOptionsWidget::PrintOptionsWidget(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    setupUi(this);
}

bool PrintOptionsWidget::printMap() const
{
    return mapCheckBox->isChecked();
}

void PrintOptionsWidget::setPrintMap(bool print)
{
    mapCheckBox->setChecked(print);
}

bool PrintOptionsWidget::printBackground() const
{
    return backgroundCheckBox->isChecked();
}

void PrintOptionsWidget::setPrintBackground(bool print)
{
    backgroundCheckBox->setChecked(print);
}

bool PrintOptionsWidget::printLegend() const
{
    return legendCheckBox->isChecked();
}

void PrintOptionsWidget::setPrintLegend(bool print)
{
    legendCheckBox->setChecked(print);
}

bool PrintOptionsWidget::printRouteSummary() const
{
    return routeSummaryCheckBox->isChecked();
}

void PrintOptionsWidget::setPrintRouteSummary(bool print)
{
    routeSummaryCheckBox->setChecked(print);
}

bool PrintOptionsWidget::printDrivingInstructions() const
{
    return drivingInstructionsCheckBox->isChecked();
}

void PrintOptionsWidget::setPrintDrivingInstructions(bool print)
{
    drivingInstructionsCheckBox->setChecked(print);
}

void PrintOptionsWidget::setBackgroundControlsEnabled(bool enabled)
{
    backgroundCheckBox->setEnabled(enabled);
}

void PrintOptionsWidget::setLegendControlsEnabled(bool enabled)
{
    legendCheckBox->setEnabled(enabled);
}

void PrintOptionsWidget::setRouteControlsEnabled(bool enabled)
{
    routeGroupBox->setEnabled(enabled);
}

bool PrintOptionsWidget::printDrivingInstructionsAdvice() const
{
    return drivingInstructionsAdviceCheckBox->isChecked();
}

void PrintOptionsWidget::setPrintDrivingInstructionsAdvice(bool enabled)
{
    drivingInstructionsAdviceCheckBox->setChecked(enabled);
}

}

#include "moc_PrintOptionsWidget.cpp"
