// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2012, 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_SEARCHRUNNER_H
#define MARBLE_SEARCHRUNNER_H

#include "marble_export.h"

#include <QObject>
#include <QVector>

namespace Marble
{

class GeoDataLatLonBox;
class GeoDataPlacemark;
class MarbleModel;

class MARBLE_EXPORT SearchRunner : public QObject
{
    Q_OBJECT

public:
    explicit SearchRunner( QObject *parent = nullptr );

    /**
     * Stores a pointer to the currently used model
     */
    void setModel( const MarbleModel *model );

    /**
     * Start a placemark search. Called by MarbleRunnerManager, runners
     * are expected to return the result via the searchFinished signal.
     * If implemented in a plugin, make sure to include Search in the
     * plugin capabilities, otherwise MarbleRunnerManager will ignore the plugin
     */
    virtual void search( const QString &searchTerm, const GeoDataLatLonBox &preferred ) = 0;

Q_SIGNALS:
    /**
     * This is emitted to indicate that the runner has finished the placemark search.
     * @param result the result of the search.
     */
    void searchFinished( const QVector<GeoDataPlacemark*>& result );

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
