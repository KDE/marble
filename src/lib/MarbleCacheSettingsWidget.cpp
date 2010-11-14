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

#include "MarbleCacheSettingsWidget.h"

#include <QtGui/QPushButton>

using namespace Marble;

MarbleCacheSettingsWidget::MarbleCacheSettingsWidget( QWidget *parent ) : QWidget( parent )
{
    setupUi( this );

    connect( button_clearVolatileCache, SIGNAL( clicked() ), SIGNAL( clearVolatileCache() ) );
    connect( button_clearPersistentCache, SIGNAL( clicked() ), SIGNAL( clearPersistentCache() ) );
    connect( kcfg_proxyAuth, SIGNAL( toggled ( bool ) ), kcfg_proxyUser, SLOT( setEnabled(bool) ) );
    connect( kcfg_proxyAuth, SIGNAL( toggled ( bool ) ), kcfg_proxyPass, SLOT( setEnabled(bool) ) );
}

#include "MarbleCacheSettingsWidget.moc"



