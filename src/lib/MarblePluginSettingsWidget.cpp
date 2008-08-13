//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn  <rahn@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "MarblePluginSettingsWidget.h"

// Qt
#include <QtGui/QStandardItemModel>

using namespace Marble;

MarblePluginSettingsWidget::MarblePluginSettingsWidget( QWidget *parent ) : QWidget( parent )
{
    setupUi( this );

    connect( m_pluginListView, SIGNAL( clicked( QModelIndex ) ),
         this, SIGNAL( pluginListViewClicked() ) );
}

void MarblePluginSettingsWidget::setModel( QStandardItemModel* pluginModel )
{
    m_pluginListView->setModel ( pluginModel );
}

#include "MarblePluginSettingsWidget.moc"
