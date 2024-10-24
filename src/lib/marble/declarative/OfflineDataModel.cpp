// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "OfflineDataModel.h"
#include "MarbleDirs.h"

#include <QDir>
#include <QModelIndex>

OfflineDataModel::OfflineDataModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_vehicleTypeFilter(Any)
{
    m_newstuffModel.setTargetDirectory(Marble::MarbleDirs::localPath() + QLatin1StringView("/maps"));
    m_newstuffModel.setRegistryFile(QDir::homePath() + QStringLiteral("/.kde/share/apps/knewstuff3/marble-offline-data.knsregistry"),
                                    Marble::NewstuffModel::NameTag);
    m_newstuffModel.setProvider(QStringLiteral("http://files.kde.org/marble/newstuff/maps-monav.xml"));

    setSourceModel(&m_newstuffModel);
    QHash<int, QByteArray> roleNames = m_newstuffModel.roleNames();
    roleNames[Qt::UserRole + 17] = "continent";
    m_roleNames = roleNames;

    sort(0);
    setDynamicSortFilter(true);

    connect(&m_newstuffModel, &Marble::NewstuffModel::installationProgressed, this, &OfflineDataModel::handleInstallationProgress);
    connect(&m_newstuffModel, &Marble::NewstuffModel::installationFinished, this, &OfflineDataModel::handleInstallationFinished);
    connect(&m_newstuffModel, &Marble::NewstuffModel::installationFailed, this, &OfflineDataModel::handleInstallationFailed);
    connect(&m_newstuffModel, &Marble::NewstuffModel::uninstallationFinished, this, &OfflineDataModel::handleUninstallationFinished);
}

int OfflineDataModel::count() const
{
    return rowCount();
}

QHash<int, QByteArray> OfflineDataModel::roleNames() const
{
    return m_roleNames;
}

QVariant OfflineDataModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && index.row() >= 0 && index.row() < rowCount() && role == Qt::DisplayRole) {
        QStringList const data = QSortFilterProxyModel::data(index, role).toString().split(QLatin1Char('/'));
        if (data.size() > 1) {
            QString result = data.at(1);
            for (int i = 2; i < data.size(); ++i) {
                result += QLatin1StringView(" / ") + data.at(i);
            }
            result.remove(QLatin1StringView(" (Motorcar)"));
            result.remove(QLatin1StringView(" (Pedestrian)"));
            result.remove(QLatin1StringView(" (Bicycle)"));
            return result.trimmed();
        }
    }

    if (index.isValid() && index.row() >= 0 && index.row() < rowCount() && role == Qt::UserRole + 17) {
        QStringList const data = QSortFilterProxyModel::data(index, Qt::DisplayRole).toString().split(QLatin1Char('/'));
        if (data.size() > 1) {
            return data.first().trimmed();
        }
    }

    return QSortFilterProxyModel::data(index, role);
}

void OfflineDataModel::setVehicleTypeFilter(VehicleTypes filter)
{
    m_vehicleTypeFilter = filter;
    beginResetModel();
    endResetModel();
}

void OfflineDataModel::install(int index)
{
    m_newstuffModel.install(toSource(index));
}

void OfflineDataModel::uninstall(int index)
{
    m_newstuffModel.uninstall(toSource(index));
}

void OfflineDataModel::cancel(int index)
{
    m_newstuffModel.cancel(toSource(index));
}

int OfflineDataModel::fromSource(int index) const
{
    return mapFromSource(m_newstuffModel.index(index)).row();
}

int OfflineDataModel::toSource(int idx) const
{
    return mapToSource(index(idx, 0)).row();
}

void OfflineDataModel::handleInstallationProgress(int index, qreal progress)
{
    Q_EMIT installationProgressed(fromSource(index), progress);
}

void OfflineDataModel::handleInstallationFinished(int index)
{
    Q_EMIT installationFinished(fromSource(index));
}

void OfflineDataModel::handleInstallationFailed(int index, const QString &error)
{
    Q_EMIT installationFailed(fromSource(index), error);
}

void OfflineDataModel::handleUninstallationFinished(int index)
{
    Q_EMIT uninstallationFinished(fromSource(index));
}

bool OfflineDataModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent)) {
        QModelIndex const index = sourceModel()->index(source_row, 0, source_parent);
        QString const data = sourceModel()->data(index, Qt::DisplayRole).toString();
        if ((m_vehicleTypeFilter & Motorcar) && data.contains(QLatin1StringView("(Motorcar)"))) {
            return true;
        } else if ((m_vehicleTypeFilter & Bicycle) && data.contains(QLatin1StringView("(Bicycle)"))) {
            return true;
        } else if ((m_vehicleTypeFilter & Pedestrian) && data.contains(QLatin1StringView("(Pedestrian)"))) {
            return true;
        }
    }

    return false;
}

#include "moc_OfflineDataModel.cpp"
