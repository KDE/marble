// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESMODEL_H
#define MARBLE_SATELLITESMODEL_H

#include <QList>
#include <QStringList>

#include "TrackerPluginModel.h"

class QVariant;

namespace Marble
{

class MarbleClock;

/**
 * The model for satellites.
 */
class SatellitesModel : public TrackerPluginModel
{
    Q_OBJECT
public:
    SatellitesModel(GeoDataTreeModel *treeModel, const MarbleClock *clock);

    void loadSettings(const QHash<QString, QVariant> &settings);
    void setPlanet(const QString &lcPlanet);
    void updateVisibility();

    void parseFile(const QString &id, const QByteArray &file) override;

protected:
    /**
     * Parse the Marble Satellite Catalog @p id with content @p data.
     * A description of the Marble Satellites Catalog format can be found at:
     * https://techbase.kde.org/Projects/Marble/SatelliteCatalogFormat
     */
    void parseCatalog(const QString &id, const QByteArray &data);
    /**
     * Parse the two line elements set file @p id with content @p data.
     */
    void parseTLE(const QString &id, const QByteArray &data);

private:
    void setupColors();
    QColor nextColor();

private:
    const MarbleClock *const m_clock;
    QStringList m_enabledIds;
    QString m_lcPlanet;
    QList<QColor> m_colorList;
    int m_currentColorIndex;
};

} // namespace Marble

#endif // MARBLE_SATELLITESMODEL_H
