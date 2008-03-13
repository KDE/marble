//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


#include "MarbleSettingsWidget.h"

#include <QtCore/QDebug>

#include "ui_MarbleSettingsWidget.h"


class MarbleSettingsWidgetPrivate
{
 public:

    Ui::MarbleSettingsWidget  uiWidget;
};


MarbleSettingsWidget::MarbleSettingsWidget( QWidget *parent )
    : QWidget( parent ),
      d( new MarbleSettingsWidgetPrivate )

{
    d->uiWidget.setupUi( this );
 
    setFocusPolicy( Qt::NoFocus );
}

MarbleSettingsWidget::~MarbleSettingsWidget()
{
    delete d;
}


#include "MarbleSettingsWidget.moc"
