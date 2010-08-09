//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_LEGENDWIDGET_H
#define MARBLE_LEGENDWIDGET_H

// Marble
#include "marble_export.h"

// Qt
#include <QtGui/QWidget>

namespace Marble
{

class LegendWidgetPrivate;

class MarbleWidget;

class MARBLE_EXPORT LegendWidget : public QWidget
{
    Q_OBJECT

 public:
    LegendWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~LegendWidget();

    /**
     * @brief Set a MarbleWidget associated to this widget.
     * @param widget  the MarbleWidget to be set.
     */
    void setMarbleWidget( MarbleWidget *widget );

 private:
    Q_DISABLE_COPY( LegendWidget )

    LegendWidgetPrivate * const d;
};

}

#endif
