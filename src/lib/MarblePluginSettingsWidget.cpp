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

// Marble
#include "PluginItemDelegate.h"

using namespace Marble;

MarblePluginSettingsWidget::MarblePluginSettingsWidget( QWidget *parent ) : QWidget( parent )
{
    setupUi( this );

    PluginItemDelegate *itemDelegate = new PluginItemDelegate( m_pluginListView, this );
    m_pluginListView->setItemDelegate( itemDelegate );
    connect( m_pluginListView, SIGNAL( clicked( QModelIndex ) ),
         this, SIGNAL( pluginListViewClicked() ) );
    connect( itemDelegate, SIGNAL( aboutPluginClicked( QString ) ),
             this, SIGNAL( aboutPluginClicked( QString ) ) );
    connect( itemDelegate, SIGNAL( configPluginClicked( QString ) ),
             this, SIGNAL( configPluginClicked( QString ) ) );
}

void MarblePluginSettingsWidget::setModel( QStandardItemModel* pluginModel )
{
    m_pluginListView->setModel ( pluginModel );
}

#include "MarblePluginSettingsWidget.moc"
