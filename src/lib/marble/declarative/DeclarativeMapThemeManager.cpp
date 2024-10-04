// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "DeclarativeMapThemeManager.h"

#include "GeoSceneHead.h"

#include <QDebug>
#include <QIcon>
#include <QStandardItemModel>

MapThemeImageProvider::MapThemeImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    // nothing to do
}

QPixmap MapThemeImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSize const resultSize = requestedSize.isValid() ? requestedSize : QSize(128, 128);
    if (size) {
        *size = resultSize;
    }

    QStandardItemModel *model = m_mapThemeManager.mapThemeModel();
    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->data(model->index(i, 0), Qt::UserRole + 1) == id) {
            auto icon = model->data(model->index(i, 0), Qt::DecorationRole).value<QIcon>();
            QPixmap result = icon.pixmap(resultSize);
            return result;
        }
    }

    QPixmap empty(resultSize);
    empty.fill();
    return empty;
}

MapThemeManager::MapThemeManager(QObject *parent)
    : QObject(parent)
{
    // nothing to do
}

QStringList MapThemeManager::mapThemeIds() const
{
    return m_mapThemeManager.mapThemeIds();
}

#include "moc_DeclarativeMapThemeManager.cpp"
