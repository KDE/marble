//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_PRINTOPTIONSWIDGET_H
#define MARBLE_PRINTOPTIONSWIDGET_H

#include "marble_export.h"

#include <QtGui/QWidget>

#include "ui_PrintOptions.h"

namespace Marble
{

class MARBLE_EXPORT PrintOptionsWidget: public QWidget, private Ui::PrintOptions
{
    Q_OBJECT

public:
    explicit PrintOptionsWidget( QWidget * parent = 0, Qt::WindowFlags f = 0 );

    bool printMap() const;

    void setPrintMap( bool print );

    bool printBackground() const;

    void setPrintBackground( bool print );

    bool printLegend() const;

    void setPrintLegend( bool print );

    bool printRouteSummary() const;

    void setPrintRouteSummary( bool print );

    bool printDrivingInstructions() const;

    void setPrintDrivingInstructions( bool print );

    bool printDrivingInstructionsAdvice() const;

    void setPrintDrivingInstructionsAdvice( bool print );

    void setBackgroundControlsEnabled( bool enabled );

    void setRouteControlsEnabled( bool enabled );

    void setLegendControlsEnabled( bool enabled );
};

} // namespace Marble

#endif // MARBLE_PRINTOPTIONSWIDGET_H
