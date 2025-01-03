// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESCONFIGNODEITEM_H
#define MARBLE_SATELLITESCONFIGNODEITEM_H

#include "SatellitesConfigAbstractItem.h"
#include <QList>

namespace Marble
{

class SatellitesConfigNodeItem : public SatellitesConfigAbstractItem
{
public:
    explicit SatellitesConfigNodeItem(const QString &name);
    ~SatellitesConfigNodeItem() override;

    void loadSettings(const QHash<QString, QVariant> &settings) override;

    QVariant data(int column, int role) const override;
    bool setData(int column, int role, const QVariant &data) override;

    bool isLeaf() const override;
    SatellitesConfigAbstractItem *childAt(int row) const override;
    int indexOf(const SatellitesConfigAbstractItem *child) const override;
    int childrenCount() const override;

    void appendChild(SatellitesConfigAbstractItem *item);
    void clear() override;

private:
    QList<SatellitesConfigAbstractItem *> m_children;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGNODEITEM_H
