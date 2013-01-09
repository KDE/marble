//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_SEARCHRUNNER_H
#define MARBLE_SEARCHRUNNER_H

#include <QtCore/QObject>
#include "marble_export.h"

#include "GeoDataDocument.h"

#include <QtCore/QVector>

namespace Marble
{

class GeoDataLatLonAltBox;
class GeoDataPlacemark;
class MarbleModel;
class RouteRequest;

class MARBLE_EXPORT SearchRunner : public QObject
{
    Q_OBJECT

public:
    explicit SearchRunner( QObject *parent = 0 );

    /**
     * Stores a pointer to the currently used model
     */
    void setModel( MarbleModel *model );

    /**
     * Start a placemark search. Called by MarbleRunnerManager, runners
     * are expected to return the result via the searchFinished signal.
     * If implemented in a plugin, make sure to include Search in the
     * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
     */
    virtual void search( const QString &searchTerm, const GeoDataLatLonAltBox &preferred ) = 0;

Q_SIGNALS:
    /**
     * This is emitted to indicate that the runner has finished the placemark search.
     * @param result the result of the search.
     */
    void searchFinished( QVector<GeoDataPlacemark*> result );

protected:
    /**
     * Access to the currently used model, or null if no was set with @see setModel
     */
    MarbleModel *model();

private:
    MarbleModel *m_model;
};

}

#endif
