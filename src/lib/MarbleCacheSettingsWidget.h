//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_MARBLECACHESETTINGSWIDGET_H
#define MARBLE_MARBLECACHESETTINGSWIDGET_H


/** @file
 * This file contains the multiple inheritance ui-wrapper for the
 * MarbleCacheSettingsWidget ui file.
 * 
 * @author Torsten Rahn  <torsten@kde.org>
 */

#include <QWidget>

#include "marble_export.h"
#include "MarbleGlobal.h"

namespace Marble
{

/** 
 * @short A public class that adds methods to the UI Cache Settings Widget.
 *
 */

class MARBLE_EXPORT MarbleCacheSettingsWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit MarbleCacheSettingsWidget( QWidget *parent = 0 );
    ~MarbleCacheSettingsWidget();

    int volatileTileCacheLimit() const;

    int persistentTileCacheLimit() const;

    QString proxyUrl() const;

    quint16 proxyPort() const;

    QString proxyUser() const;

    QString proxyPassword() const;

    ProxyType proxyType() const;

    bool isProxyAuthenticationEnabled() const;

 public Q_SLOTS:
    void setVolatileTileCacheLimit( int volatileTileCacheLimit );

    void setPersistentTileCacheLimit( int persistentTileCacheLimit );

    void setProxyUrl( const QString &proxyUrl );

    void setProxyPort( quint16 proxyPort );

    void setProxyUser( const QString &proxyUser );

    void setProxyPassword( const QString &proxyPassword );

    void setProxyType( ProxyType proxyType );

    void setProxyAuthenticationEnabled( bool enabled );

 Q_SIGNALS:
    void clearVolatileCache();
    void clearPersistentCache();

private:
    class Private;
    Private *d;
};

}

#endif
