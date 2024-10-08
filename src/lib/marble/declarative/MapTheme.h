// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2022 Torsten Rahn <rahn@kde.org>
//

#ifndef MARBLE_MAPTHEME_H
#define MARBLE_MAPTHEME_H

#include <QObject>
#include <QQuickItem>

Q_MOC_INCLUDE("MarbleQuickItem.h")

/**
 * Represents the maptheme with its most important properties
 *
 * @todo: Expose more properties
 */
namespace Marble
{
class MarbleQuickItem;

class MapTheme : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Marble::MarbleQuickItem *map READ map WRITE setMap NOTIFY mapChanged)

    Q_PROPERTY(QString license READ license NOTIFY licenseChanged)

public:
    explicit MapTheme(QObject *parent = nullptr);

    /** Query the Marble map backend that this item uses for screen position determination */
    MarbleQuickItem *map() const;

    QString license() const;

    /** Hook up the GeoItem with Marble's map backend */
    void setMap(MarbleQuickItem *map);
Q_SIGNALS:
    void mapChanged(MarbleQuickItem *);
    void licenseChanged();

private:
    MarbleQuickItem *m_map = nullptr;
};
}

#endif // MARBLE_MAPTHEME_H
