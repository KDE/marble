// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_STREETMAPTHEMEMODEL_H
#define MARBLE_DECLARATIVE_STREETMAPTHEMEMODEL_H

#include <QSortFilterProxyModel>
#include <qqmlregistration.h>

namespace Marble
{
class MapThemeManager;
}

class MapThemeModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum MapThemeFilter {
        AnyTheme = 0x0,
        Terrestrial = 0x1,
        Extraterrestrial = 0x2,
        LowZoom = 0x4,
        HighZoom = 0x8,
    };

    Q_DECLARE_FLAGS(MapThemeFilters, MapThemeFilter)

    Q_FLAGS(MapThemeFilter MapThemeFilters)

    Q_PROPERTY(MapThemeFilters mapThemeFilter READ mapThemeFilter WRITE setMapThemeFilter NOTIFY mapThemeFilterChanged)

public:
    explicit MapThemeModel(QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QString name(const QString &id) const;

    Q_INVOKABLE int indexOf(const QString &id) const;

    Q_INVOKABLE MapThemeFilters mapThemeFilter() const;

public Q_SLOTS:
    void setMapThemeFilter(MapThemeFilters filters);

Q_SIGNALS:
    void mapThemeFilterChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private Q_SLOTS:
    void handleChangedThemes();

private:
    Marble::MapThemeManager *m_themeManager;

    QList<QString> m_streetMapThemeIds;

    MapThemeFilters m_mapThemeFilters;
};

#endif
