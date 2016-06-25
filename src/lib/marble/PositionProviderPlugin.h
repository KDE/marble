//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef MARBLE_POSITIONPROVIDERPLUGIN_H
#define MARBLE_POSITIONPROVIDERPLUGIN_H

#include "PositionProviderPluginInterface.h"
#include "marble_export.h"


namespace Marble
{
class PositionProviderPluginPrivate;

/**
 * @short The abstract class that provides position information.
 */
class MARBLE_EXPORT PositionProviderPlugin : public QObject, public PositionProviderPluginInterface
{
    Q_OBJECT

 public:
    virtual ~PositionProviderPlugin();

    /**
     * @brief Returns the string that should appear in the user interface.
     *
     * Example: "GPS"
     */
    virtual QString guiString() const = 0;

    /**
     * Create a new PositionProvider Plugin and return it.
     * Has to be defined in concrete position provider plugin classes.
     */
    virtual PositionProviderPlugin * newInstance() const = 0;

 Q_SIGNALS:
    void statusChanged( PositionProviderStatus status ) const;
    void positionChanged( const GeoDataCoordinates& position,
                          const GeoDataAccuracy& accuracy ) const;

 protected:
    PositionProviderPlugin();

 private:
    Q_DISABLE_COPY( PositionProviderPlugin )
    PositionProviderPluginPrivate *d;

};

}

#endif
