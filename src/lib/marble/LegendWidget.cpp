// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "LegendWidget.h"

// Marble
#include "MarbleModel.h"

using namespace Marble;
// Ui
#ifdef MARBLE_NO_WEBKITWIDGETS
#include "ui_NullLegendWidget.h"
#else
#include "ui_LegendWidget.h"
#endif

namespace Marble
{

class LegendWidgetPrivate : public Ui::LegendWidget
{
public:
    LegendWidgetPrivate();
};

LegendWidgetPrivate::LegendWidgetPrivate() = default;

LegendWidget::LegendWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , d(new LegendWidgetPrivate)
{
    d->setupUi(this);
    layout()->setContentsMargins({});
    connect(d->m_marbleLegendBrowser, SIGNAL(tourLinkClicked(QString)), this, SIGNAL(tourLinkClicked(QString)));
}

LegendWidget::~LegendWidget()
{
    delete d;
}

void LegendWidget::setMarbleModel(MarbleModel *model)
{
    // Initialize the MarbleLegendBrowser
    d->m_marbleLegendBrowser->setMarbleModel(model);

    // connect signals for the Legend
    connect(d->m_marbleLegendBrowser, SIGNAL(toggledShowProperty(QString, bool)), this, SIGNAL(propertyValueChanged(QString, bool)));
}

}

#include "moc_LegendWidget.cpp"
