//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "LegendWidget.h"

// Marble
#include "MarbleWidget.h"

using namespace Marble;
// Ui
#include "ui_LegendWidget.h"

namespace Marble
{

class LegendWidgetPrivate
{
 public:
    Ui::LegendWidget    m_legendUi;
    MarbleWidget       *m_widget;
};

LegendWidget::LegendWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new LegendWidgetPrivate() )
{
    d->m_legendUi.setupUi( this );
}

LegendWidget::~LegendWidget()
{
    delete d;
}

void LegendWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;

    // Initialize the MarbleLegendBrowser
    d->m_legendUi.marbleLegendBrowser->setMarbleWidget( d->m_widget );

    // connect signals for the Legend
    connect( d->m_legendUi.marbleLegendBrowser, SIGNAL( toggledShowProperty( QString, bool ) ),
             d->m_widget,                       SLOT( setPropertyValue( QString, bool ) ) );
}

}

#include "LegendWidget.moc"
