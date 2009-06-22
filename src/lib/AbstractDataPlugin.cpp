//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "AbstractDataPlugin.h"

// Marble
#include "AbstractDataPluginModel.h"
#include "AbstractDataPluginItem.h"
#include "GeoPainter.h"
#include "GeoSceneLayer.h"
#include "ViewportParams.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>

namespace Marble {

class AbstractDataPluginPrivate
{
 public:
    AbstractDataPluginPrivate()
        : m_model( 0 ),
          m_numberOfItems( 10 )
    {
    }
    
    ~AbstractDataPluginPrivate()
    {
    }
    
    AbstractDataPluginModel *m_model;
    QString m_name;
    quint32 m_numberOfItems;
};

AbstractDataPlugin::AbstractDataPlugin()
    : RenderPlugin(),
      d( new AbstractDataPluginPrivate )
{
}

AbstractDataPlugin::~AbstractDataPlugin()
{
    delete d;
}

QStringList AbstractDataPlugin::backendTypes() const
{
    return QStringList( name() );
}

QString AbstractDataPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList AbstractDataPlugin::renderPosition() const
{
    return QStringList( "HOVERS_ABOVE_SURFACE" );
}

bool AbstractDataPlugin::isInitialized() const
{
    return true;
}

bool AbstractDataPlugin::render( GeoPainter *painter, ViewportParams *viewport,
             const QString& renderPos, GeoSceneLayer * layer)
{
    if ( ( 0 == d->m_model ) || !isInitialized() ) {
        return true;
    }
    
    if ( renderPos != "HOVERS_ABOVE_SURFACE" ) {
        return true;
    }

    QList<AbstractDataPluginItem*> items = d->m_model->items( viewport,
                                                              dataFacade(),
                                                              numberOfItems() );
    painter->save();
    
    // Paint the most important item at last
    for( int i = items.size() - 1; i >= 0; --i ) {
        items.at( i )->paintEvent( painter, viewport, renderPos, layer );
    }
    
    painter->restore();
    
    return true;
}

AbstractDataPluginModel *AbstractDataPlugin::model() const
{
    return d->m_model;
}

void AbstractDataPlugin::setModel( AbstractDataPluginModel* model )
{
    d->m_model = model;
}

QString AbstractDataPlugin::nameId() const
{
    return d->m_name;
}
    
void AbstractDataPlugin::setNameId( const QString& name )
{
    d->m_name = name;
}


quint32 AbstractDataPlugin::numberOfItems() const
{
    return d->m_numberOfItems;
}
    
void AbstractDataPlugin::setNumberOfItems( quint32 number )
{
    d->m_numberOfItems = number;
}

QList<AbstractDataPluginItem *> AbstractDataPlugin::whichItemAt( const QPoint& curpos )
{
    if ( enabled() && visible() ) {
        return d->m_model->whichItemAt( curpos );
    }
    else {
        return QList<AbstractDataPluginItem *>();
    }
}

} // namespace Marble

#include "AbstractDataPlugin.moc"
