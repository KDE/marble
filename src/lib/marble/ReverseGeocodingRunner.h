// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2012, 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_REVERSEGEOCODINGRUNNER_H
#define MARBLE_REVERSEGEOCODINGRUNNER_H

#include <QObject>
#include "marble_export.h"

namespace Marble
{

class GeoDataCoordinates;
class GeoDataPlacemark;
class MarbleModel;

class MARBLE_EXPORT ReverseGeocodingRunner : public QObject
{
    Q_OBJECT

public:
    explicit ReverseGeocodingRunner( QObject *parent );

    /**
     * Stores a pointer to the currently used model
     */
    void setModel( const MarbleModel *model );

    /**
     * Start a reverse geocoding request. Called by MarbleRunnerManager, runners
     * are expected to return the result via the reverseGeocodingFinished signal.
     * If implemented in a plugin, make sure to include ReverseGeocoding in the
     * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
     */
    virtual void reverseGeocoding( const GeoDataCoordinates &coordinates ) = 0;

Q_SIGNALS:
    /**
     * Reverse geocoding is finished, result in the given placemark.
     * To be emitted by runners after a @see reverseGeocoding call.
     */
    void reverseGeocodingFinished( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark );

protected:
    /**
     * Access to the currently used model, or null if no was set with @see setModel
     */
    const MarbleModel *model() const;

private:
    const MarbleModel *m_model;
};

}

#endif
