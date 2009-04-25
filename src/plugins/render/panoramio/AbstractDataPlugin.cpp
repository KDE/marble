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
#include "AbstractDataPluginWidget.h"
#include "GeoPainter.h"
#include "GeoSceneLayer.h"
#include "ViewportParams.h"

namespace Marble {

class AbstractDataPluginPrivate
{
 public:
    AbstractDataPluginPrivate()
        : m_numberOfWidgets( 10 )
    {
    }
    
    AbstractDataPluginModel *m_model;
    QString m_name;
    quint32 m_numberOfWidgets;
};

AbstractDataPlugin::AbstractDataPlugin()
    : RenderPlugin(),
      d( new AbstractDataPluginPrivate )
{
}

QStringList AbstractDataPlugin::backendTypes() const {
    return QStringList( name() );
}

QString AbstractDataPlugin::renderPolicy() const {
    return QString( "ALWAYS" );
}

QStringList AbstractDataPlugin::renderPosition() const {
    return QStringList( "ALWAYS_ON_TOP" );
}

bool AbstractDataPlugin::isInitialized() const {
    return true;
}

bool AbstractDataPlugin::render( GeoPainter *painter, ViewportParams *viewport,
             const QString& renderPos, GeoSceneLayer * layer)
{
    QList<AbstractDataPluginWidget*> widgets = d->m_model->widgets( viewport,
                                                                 numberOfWidgets() );
    painter->save();
    
    QList<AbstractDataPluginWidget*>::iterator it;
    
    for( it = widgets.begin(); it != widgets.end(); ++it ) {
        (*it)->render( painter, viewport, renderPos, layer );
    }
    
    painter->restore();
    
    return true;
}

AbstractDataPluginModel *AbstractDataPlugin::model() const {
    return d->m_model;
}

void AbstractDataPlugin::setModel( AbstractDataPluginModel* model ) {
    d->m_model = model;
}

QString AbstractDataPlugin::nameId() const {
    return d->m_name;
}
    
void AbstractDataPlugin::setNameId( QString name ) {
    d->m_name = name;
}


quint32 AbstractDataPlugin::numberOfWidgets() const {
    return d->m_numberOfWidgets;
}
    
void AbstractDataPlugin::setNumberOfWidgets( quint32 number ) {
    d->m_numberOfWidgets = number;
}

} // namespace Marble

#include "AbstractDataPlugin.moc"
