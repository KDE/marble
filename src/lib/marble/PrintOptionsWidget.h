// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_PRINTOPTIONSWIDGET_H
#define MARBLE_PRINTOPTIONSWIDGET_H

#include "marble_export.h"

#include <QWidget>

#include "ui_PrintOptions.h"

namespace Marble
{

class MARBLE_EXPORT PrintOptionsWidget : public QWidget, private Ui::PrintOptions
{
    Q_OBJECT

public:
    explicit PrintOptionsWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    bool printMap() const;

    void setPrintMap(bool print);

    bool printBackground() const;

    void setPrintBackground(bool print);

    bool printLegend() const;

    void setPrintLegend(bool print);

    bool printRouteSummary() const;

    void setPrintRouteSummary(bool print);

    bool printDrivingInstructions() const;

    void setPrintDrivingInstructions(bool print);

    bool printDrivingInstructionsAdvice() const;

    void setPrintDrivingInstructionsAdvice(bool print);

    void setBackgroundControlsEnabled(bool enabled);

    void setRouteControlsEnabled(bool enabled);

    void setLegendControlsEnabled(bool enabled);
};

} // namespace Marble

#endif // MARBLE_PRINTOPTIONSWIDGET_H
