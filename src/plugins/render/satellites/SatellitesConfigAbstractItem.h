//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESCONFIGABSTRACTITEM_H
#define MARBLE_SATELLITESCONFIGABSTRACTITEM_H

#include <QHash>
#include <QString>

class QVariant;

namespace Marble
{

class SatellitesConfigAbstractItem
{
public:
    enum {
        IdListRole      = Qt::UserRole + 0,
        FullIdListRole  = Qt::UserRole + 1,
        UrlListRole     = Qt::UserRole + 2
    };

    virtual ~SatellitesConfigAbstractItem();

    QString name() const;

    int row() const;

    SatellitesConfigAbstractItem *parent() const;
    void setParent( SatellitesConfigAbstractItem *parent );

    virtual void loadSettings(const QHash<QString, QVariant> &settings);

    virtual QVariant data( int column, int role ) const;
    virtual bool setData( int column, int role, const QVariant &data );
    
    virtual bool isLeaf() const = 0;
    virtual SatellitesConfigAbstractItem *childAt( int row ) const = 0;
    virtual int indexOf( const SatellitesConfigAbstractItem *child ) const = 0;
    virtual int childrenCount() const = 0;

    virtual Qt::ItemFlags flags() const;
    virtual void setFlags( Qt::ItemFlags flags );

    virtual void clear();

protected:
    explicit SatellitesConfigAbstractItem( const QString &name );

private:
    QString m_name;
    SatellitesConfigAbstractItem *m_parent;
    Qt::ItemFlags m_flags;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGABSTRACTITEM_H
