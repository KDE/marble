//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn  <rahn@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "MarblePluginSettingsWidget.h"

// Qt
#include <QtGui/QStandardItemModel>

// Marble
#include "MarbleDebug.h"
#include "PluginItemDelegate.h"

namespace Marble
{

class MarblePluginSettingsWidgetPrivate
{
 public:
    PluginItemDelegate *m_itemDelegate;
};

MarblePluginSettingsWidget::MarblePluginSettingsWidget( QWidget *parent )
    : QWidget( parent ),
      d( new MarblePluginSettingsWidgetPrivate )
{
    setupUi( this );

    d->m_itemDelegate = new PluginItemDelegate( m_pluginListView, this );
    m_pluginListView->setItemDelegate( d->m_itemDelegate );
    connect( m_pluginListView, SIGNAL( clicked( QModelIndex ) ),
         this, SIGNAL( pluginListViewClicked() ) );
    connect( d->m_itemDelegate, SIGNAL( aboutPluginClicked( QString ) ),
             this, SIGNAL( aboutPluginClicked( QString ) ) );
    connect( d->m_itemDelegate, SIGNAL( configPluginClicked( QString ) ),
             this, SIGNAL( configPluginClicked( QString ) ) );
}

void MarblePluginSettingsWidget::setAboutIcon( const QIcon& icon )
{
    d->m_itemDelegate->setAboutIcon( icon );
}

MarblePluginSettingsWidget::~MarblePluginSettingsWidget()
{
    delete d;
}

void MarblePluginSettingsWidget::setConfigIcon( const QIcon& icon )
{
    d->m_itemDelegate->setConfigIcon( icon );
}

void MarblePluginSettingsWidget::setModel( QStandardItemModel* pluginModel )
{
    m_pluginListView->setModel ( pluginModel );
}

} // namespace Marble

#include "MarblePluginSettingsWidget.moc"
