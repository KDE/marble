//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESMODEL_H
#define MARBLE_SATELLITESMODEL_H

#include <QStringList>
#include <QVector>

#include "TrackerPluginModel.h"

class QVariant;

namespace Marble {

class MarbleClock;

/**
 * The model for satellites.
 */
class SatellitesModel : public TrackerPluginModel
{
    Q_OBJECT
public:
    SatellitesModel( GeoDataTreeModel *treeModel,
                     const MarbleClock *clock );

    void loadSettings( const QHash<QString, QVariant> &settings );
    void setPlanet( const QString &lcPlanet );
    void updateVisibility();

    void parseFile( const QString &id, const QByteArray &file );

protected:
    /**
     * Parse the Marble Satellite Catalog @p id with content @p data.
     * A description of the Marble Satellites Catalog format can be found at:
     * http://techbase.kde.org/Projects/Marble/SatelliteCatalogFormat
     */
    void parseCatalog( const QString &id, const QByteArray &data );
    /**
     * Parse the two line elements set file @p id with content @p data.
     */
    void parseTLE( const QString &id, const QByteArray &data );

private:
    void setupColors();
    QColor nextColor();

private:
    const MarbleClock *m_clock;
    QStringList m_enabledIds;
    QString m_lcPlanet;
    QVector<QColor> m_colorList;
    int m_currentColorIndex;
};

} // namespace Marble

#endif // MARBLE_SATELLITESMODEL_H
