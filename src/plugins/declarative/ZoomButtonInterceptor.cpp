//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "ZoomButtonInterceptor.h"

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>

#ifdef HARMATTAN_ZOOMINTERCEPTOR
#include <policy/resource-set.h>
#endif // HARMATTAN_ZOOMINTERCEPTOR

#include "MarbleWidget.h"

namespace Marble
{
namespace Declarative
{

class ZoomButtonInterceptorPrivate
{
public:
    ZoomButtonInterceptorPrivate( Marble::MarbleWidget* widget );

    ~ZoomButtonInterceptorPrivate();

    Marble::MarbleWidget* m_widget;

#ifdef HARMATTAN_ZOOMINTERCEPTOR
    ResourcePolicy::ResourceSet* m_resourceSet;
#endif //HARMATTAN_ZOOMINTERCEPTOR
};

ZoomButtonInterceptorPrivate::ZoomButtonInterceptorPrivate(MarbleWidget *widget)
    : m_widget( widget )
#ifdef HARMATTAN_ZOOMINTERCEPTOR
     , m_resourceSet( new ResourcePolicy::ResourceSet( "player" ) )
#endif //HARMATTAN_ZOOMINTERCEPTOR
{
#ifdef HARMATTAN_ZOOMINTERCEPTOR
    m_resourceSet->addResourceObject( new ResourcePolicy::ScaleButtonResource );
    m_resourceSet->acquire();
#endif //HARMATTAN_ZOOMINTERCEPTOR
}

ZoomButtonInterceptorPrivate::~ZoomButtonInterceptorPrivate()
{
#ifdef HARMATTAN_ZOOMINTERCEPTOR
    m_resourceSet->deleteResource( ResourcePolicy::ScaleButtonType );
    delete m_resourceSet;
#endif //HARMATTAN_ZOOMINTERCEPTOR
}

bool ZoomButtonInterceptor::eventFilter(QObject *, QEvent *event)
{
#ifdef HARMATTAN_ZOOMINTERCEPTOR
    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );
        qDebug() << "evaluating key " << keyEvent->key();
        if ( keyEvent->key() == Qt::Key_VolumeDown ) {
            d->m_widget->zoomOut();
            return true;
        }

        if ( keyEvent->key() == Qt::Key_VolumeUp ) {
            d->m_widget->zoomIn();
            return true;
        }
    } else if ( event->type() == QEvent::ApplicationDeactivate ) {
        d->m_resourceSet->release();
    } else if ( event->type() == QEvent::ApplicationActivate ) {
        d->m_resourceSet->acquire();
    }
#else
    Q_UNUSED( event )
#endif // HARMATTAN_ZOOMINTERCEPTOR

    return false;
}

ZoomButtonInterceptor::ZoomButtonInterceptor( Marble::MarbleWidget *widget, QObject *parent )
    : QObject( parent ),
    d( new ZoomButtonInterceptorPrivate( widget ) )
{
    QApplication::instance()->installEventFilter( this );
}

ZoomButtonInterceptor::~ZoomButtonInterceptor()
{
    delete d;
}

}
}

#include "ZoomButtonInterceptor.moc"
