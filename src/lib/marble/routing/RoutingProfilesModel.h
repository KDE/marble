// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_ROUTINGPROFILESMODEL_H
#define MARBLE_ROUTINGPROFILESMODEL_H

#include "RoutingProfile.h"
#include "marble_export.h"

#include <QAbstractListModel>

namespace Marble
{

class PluginManager;

class MARBLE_EXPORT RoutingProfilesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit RoutingProfilesModel(const PluginManager *pluginManager, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    enum ProfileTemplate {
        CarFastestTemplate,
        CarShortestTemplate,
        CarEcologicalTemplate,
        BicycleTemplate,
        PedestrianTemplate,

        LastTemplate
    };

    void setProfiles(const QList<RoutingProfile> &profiles);
    QList<RoutingProfile> profiles() const;

    void loadDefaultProfiles();

    void addProfile(const QString &name);
    bool moveUp(int row);
    bool moveDown(int row);

    bool setProfileName(int row, const QString &name);
    bool setProfilePluginSettings(int row, const QHash<QString, QHash<QString, QVariant>> &pluginSettings);

private:
    QList<RoutingProfile> m_profiles;
    const PluginManager *m_pluginManager;
};

}

#endif // MARBLE_ROUTINGPROFILESMODEL_H
