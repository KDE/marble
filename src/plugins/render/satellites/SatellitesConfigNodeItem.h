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

namespace Marble
{

class SatellitesConfigNodeItem : public SatellitesConfigAbstractItem
{
public:
    explicit SatellitesConfigNodeItem( const QString &name );
    ~SatellitesConfigNodeItem();

    void loadSettings( QHash<QString, QVariant> settings );

    QVariant data( int column, int role ) const;
    bool setData(int column, int role, const QVariant& data);

    bool isLeaf() const;
    SatellitesConfigAbstractItem *childAt( int row ) const;
    int indexOf( const SatellitesConfigAbstractItem *child ) const;
    int childrenCount() const;

    void appendChild( SatellitesConfigAbstractItem *item );

private:
    QVector<SatellitesConfigAbstractItem *> m_children;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGNODEITEM_H
