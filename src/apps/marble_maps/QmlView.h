//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

#ifndef MARBLE_QMLVIEW_H
#define MARBLE_QMLVIEW_H 

#include <QQuickView>

namespace Marble
{

class QmlView : public QQuickView
{
public:
    QmlView();

    /**
     * @brief It loads the main qml file, and it inits the qml application.
     */
    void start();
};

}
#endif
