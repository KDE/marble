/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENESECTION_H
#define MARBLE_GEOSCENESECTION_H

#include <QList>
#include <QString>

#include "GeoDocument.h"

namespace Marble
{

class GeoSceneItem;

/**
 * @short Section of a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneSection : public GeoNode
{
public:
    explicit GeoSceneSection(const QString &name);
    ~GeoSceneSection() override;

    const char *nodeType() const override;

    /**
     * @brief  Add an item to the legend section
     * @param  item  the new item
     */
    void addItem(GeoSceneItem *);
    GeoSceneItem *item(const QString &);
    QList<GeoSceneItem *> items() const;

    QString name() const;

    QString heading() const;
    void setHeading(const QString &heading);

    bool checkable() const;
    void setCheckable(bool checkable);

    QString connectTo() const;
    void setConnectTo(const QString &text);

    int spacing() const;
    void setSpacing(int spacing);

    QString radio() const;
    void setRadio(const QString &radio);

private:
    Q_DISABLE_COPY(GeoSceneSection)

    /// The vector holding all the items in the legend section.
    /// (We want to preserve the order and don't care
    /// much about speed here), so we don't use a hash
    QList<GeoSceneItem *> m_items;

    QString m_name;
    QString m_heading;
    QString m_connectTo;
    QString m_radio;

    bool m_checkable;
    int m_spacing;
};

}

#endif
