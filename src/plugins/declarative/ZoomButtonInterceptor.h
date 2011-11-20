//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_ZOOMBUTTONINTERCEPTOR_H
#define MARBLE_DECLARATIVE_ZOOMBUTTONINTERCEPTOR_H

#include <QtCore/QObject>

namespace Marble
{

class MarbleWidget;

namespace Declarative
{

class ZoomButtonInterceptorPrivate;

class ZoomButtonInterceptor : public QObject
{
    Q_OBJECT

public:
    explicit ZoomButtonInterceptor( Marble::MarbleWidget* widget, QObject* parent );

    ~ZoomButtonInterceptor();

protected:
    virtual bool eventFilter(QObject *, QEvent *event);

private:
    ZoomButtonInterceptorPrivate* const d;
};

}
}

#endif // MARBLE_DECLARATIVE_ZOOMBUTTONINTERCEPTOR_H
