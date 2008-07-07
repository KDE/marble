//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//


#include "MarbleAbstractLayer.h"


#include <QtGui/QAction>
#include <QtCore/QDebug>
#include <QtGui/QStandardItem>

class MarbleAbstractLayerPrivate
{
  public:
    MarbleAbstractLayerPrivate()
        : m_action(0),
          m_item(0),
          m_enabled(true),
          m_visible(true)
    {
    }

    ~MarbleAbstractLayerPrivate()
    {
    }

    QAction            *m_action;
    QStandardItem      *m_item;

    bool                m_enabled;
    bool                m_visible;
};


MarbleAbstractLayer::MarbleAbstractLayer()
    : d( new MarbleAbstractLayerPrivate() )
{
    d->m_action = new QAction( this );
    connect( d->m_action, SIGNAL( toggled( bool ) ), this, SLOT( setVisible( bool ) ) );
    d->m_item = new QStandardItem();
}

MarbleAbstractLayer::~MarbleAbstractLayer()
{
    delete d;
}

QAction* MarbleAbstractLayer::action() const
{
    d->m_action->setCheckable( true );
    d->m_action->setChecked( visible() );
    d->m_action->setIcon( icon() );
    d->m_action->setText( guiString() );
    d->m_action->setToolTip( description() );
    return d->m_action;
}

QStandardItem* MarbleAbstractLayer::item() const
{
    d->m_item->setIcon( icon() );
    d->m_item->setText( name() );
    d->m_item->setEditable( false );
    d->m_item->setCheckable( true );
    d->m_item->setCheckState( enabled() ?  Qt::Checked : Qt::Unchecked  );

    d->m_item->setToolTip( description() );
 

    return d->m_item;
}

void MarbleAbstractLayer::applyItemState()
{
    d->m_enabled = ( d->m_item->checkState() == Qt::Checked ) ? true : false;

    d->m_action->setEnabled( d->m_enabled );
}

void MarbleAbstractLayer::retrieveItemState()
{
    d->m_item->setCheckState( enabled() ?  Qt::Checked : Qt::Unchecked  );
}

void MarbleAbstractLayer::setEnabled( bool enabled )
{
    if ( enabled == d->m_enabled )
    {
        return;
    }
    d->m_enabled = enabled;
    d->m_action->setEnabled( enabled );

    d->m_item->setCheckState( enabled ?  Qt::Checked : Qt::Unchecked  );
}

void MarbleAbstractLayer::setVisible( bool visible )
{
    if ( visible == d->m_visible )
    {
        return;
    }
    d->m_visible = visible;
    d->m_action->setChecked( visible );

    emit valueChanged( nameId(), visible );
}

bool MarbleAbstractLayer::enabled() const
{
    return d->m_enabled;
}

bool MarbleAbstractLayer::visible() const
{
    return d->m_visible;
}

#include "MarbleAbstractLayer.moc"

