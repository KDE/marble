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

#include <QVector>
#include <QStringList>

namespace Marble
{

class SatellitesConfigAbstractItem
{
public:
    enum {
        UrlListRole         = Qt::UserRole + 0,
        OrbiterDataListRole = Qt::UserRole + 1
    };

    virtual ~SatellitesConfigAbstractItem();

    QString name() const;

    int row() const;

    SatellitesConfigAbstractItem *parent() const;
    void setParent( SatellitesConfigAbstractItem *parent );

    virtual void loadSettings( QHash<QString, QVariant> settings );

    virtual QVariant data( int column, int role ) const;
    virtual bool setData( int column, int role, const QVariant &data );
    
    virtual bool isLeaf() const = 0;
    virtual SatellitesConfigAbstractItem *childAt( int row ) const = 0;
    virtual int indexOf( const SatellitesConfigAbstractItem *child ) const = 0;
    virtual int childrenCount() const = 0;

    virtual void clear();

protected:
    explicit SatellitesConfigAbstractItem( const QString &name );

private:
    QString m_name;
    SatellitesConfigAbstractItem *m_parent;

    bool m_checked;
};

} // namespace Marble

#endif // MARBLE_SATELLITESCONFIGABSTRACTITEM_H
