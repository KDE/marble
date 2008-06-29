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

class MarbleAbstractLayerPrivate
{
  public:
    MarbleAbstractLayerPrivate()
        : m_action(0),
          m_visible(true)
    {
    }

    ~MarbleAbstractLayerPrivate()
    {
    }

    QAction            *m_action;
    bool                m_visible;
};


MarbleAbstractLayer::MarbleAbstractLayer()
    : d( new MarbleAbstractLayerPrivate() )
{
    d->m_action = new QAction( this );
    connect( d->m_action, SIGNAL( toggled( bool ) ), this, SLOT( setVisible( bool ) ) );
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

bool MarbleAbstractLayer::visible() const
{
    return d->m_visible;
}

#include "MarbleAbstractLayer.moc"

