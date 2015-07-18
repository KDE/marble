//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

#include "QmlView.h"
#include "MarbleQuickItem.h"

using namespace Marble;

QmlView::QmlView():
    QQuickView()
{
}

void QmlView::start()
{
    qmlRegisterType<MarbleQuickItem>("MarbleItem", 1, 0, "MarbleItem");
    setSource(QUrl("qrc:/MainScreen.qml"));

    if( status() != QQuickView::Ready )
    {
        qDebug() << "QmlView initialisation has failed";
    }

    setColor(QColor(Qt::black));
    setTitle("Marble Maps");

    showFullScreen();
}

#include "QmlView.h"
