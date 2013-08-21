//
// This file is part of the Marble Virtual Globe.
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
#include "MarbleModel.h"

using namespace Marble;
// Ui
#include "ui_LegendWidget.h"

#include "MarbleLegendBrowser.h"

#include <QNetworkAccessManager>

namespace Marble
{

class LegendWidgetPrivate
{
 public:
    LegendWidgetPrivate(LegendWidget *myself);
    Ui::LegendWidget    m_legendUi;
    QNetworkAccessManager m_networkAccessManager;
    MarbleLegendBrowser *m_marbleLegendBrowser;
};

LegendWidgetPrivate::LegendWidgetPrivate(LegendWidget *myself)
{
    m_legendUi.setupUi( myself );
    m_marbleLegendBrowser = new MarbleLegendBrowser(myself);
    m_legendUi.verticalLayout->addWidget(m_marbleLegendBrowser);

    // prevent triggering of network requests under Maemo, presumably due to qrc: URLs
    m_networkAccessManager.setNetworkAccessible( QNetworkAccessManager::NotAccessible );
    m_marbleLegendBrowser->page()->setNetworkAccessManager( &m_networkAccessManager );
}

LegendWidget::LegendWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new LegendWidgetPrivate(this) )
{
    layout()->setMargin( 0 );
}

LegendWidget::~LegendWidget()
{
    delete d;
}

void LegendWidget::setMarbleModel( MarbleModel *model )
{
    // Initialize the MarbleLegendBrowser
    d->m_marbleLegendBrowser->setMarbleModel( model );

    // connect signals for the Legend
    connect( d->m_marbleLegendBrowser, SIGNAL(toggledShowProperty(QString,bool)),
             this,                            SIGNAL(propertyValueChanged(QString,bool)) );
}

}

#include "LegendWidget.moc"
