//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESCONFIGNODEITEM_H
#define MARBLE_SATELLITESCONFIGNODEITEM_H

#include "SatellitesConfigAbstractItem.h"
#include <QVector>

namespace Marble
{

class SatellitesConfigNodeItem : public SatellitesConfigAbstractItem
{
public:
    explicit SatellitesConfigNodeItem( const QString &name );
    ~SatellitesConfigNodeItem() override;

    void loadSettings(const QHash<QString, QVariant> &settings) override;

    QVariant data( int column, int role ) const override;
    bool setData(int column, int role, const QVariant& data) override;

    bool isLeaf() const override;
    SatellitesConfigAbstractItem *childAt( int row ) const override;
    int indexOf( const SatellitesConfigAbstractItem *child ) const override;
    int childrenCount() const override;

    void appendChild( SatellitesConfigAbstractItem *item );
    void clear() override;

private:
    QVector<SatellitesConfigAbstractItem *> m_children;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGNODEITEM_H
