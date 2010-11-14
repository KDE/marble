/**
 * This file is part of the Marble Virtual Globe.
 *
 * Copyright (C) 2005 Torsten Rahn (rahn@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MARBLE_MARBLEVIEWADAPTOR_H
#define MARBLE_MARBLEVIEWADAPTOR_H

#include <QtDBus/QDBusAbstractAdaptor>

#include "katlasview.h"



/**
 * @short D-Bus Adaptor for MarbleView
 *
 * This class provides a D-Bus Adaptor for the MarbleView.
 *
 * @author Torsten Rahn rahn @ kde.org
 */

class MarbleViewAdaptor : public QDBusAbstractAdaptor
{

    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.DBus.MarbleView")

 public:
    explicit MarbleViewAdaptor( KAtlasView* marbleView )
        : QDBusAbstractAdaptor(marbleView)
    {
    }

 protected:
    KAtlasView *m_marbleView;

public slots:
//    Q_ASYNC 


    void zoomIn()
    {
        m_marbleView->zoomIn();
    }

};

#endif // MARBLE_MARBLEVIEWADAPTOR_H

