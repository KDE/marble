// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
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
    ~PositionProviderPlugin() override;

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
    PositionProviderPlugin(QObject* parent=nullptr);

 private:
    Q_DISABLE_COPY( PositionProviderPlugin )
    PositionProviderPluginPrivate *d;

};

}

#endif
