/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEITEM_H
#define MARBLE_GEOSCENEITEM_H

#include <QString>

#include "GeoDocument.h"

namespace Marble
{

class GeoSceneIcon;

/**
 * @short The section item in a legend of a GeoScene document.
 */

class GEODATA_EXPORT GeoSceneItem : public GeoNode
{
public:
    explicit GeoSceneItem(const QString &name);
    ~GeoSceneItem() override;

    const char *nodeType() const override;

    QString name() const;

    QString text() const;
    void setText(const QString &text);

    bool checkable() const;
    void setCheckable(bool checkable);

    QString connectTo() const;
    void setConnectTo(const QString &text);

    int spacing() const;
    void setSpacing(int spacing);

    const GeoSceneIcon *icon() const;
    GeoSceneIcon *icon();

private:
    Q_DISABLE_COPY(GeoSceneItem)

    GeoSceneIcon *const m_icon;

    QString m_name;
    QString m_text;
    QString m_connectTo;

    bool m_checkable;
    int m_spacing;
};

}

#endif
