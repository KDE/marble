//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_STREETMAPTHEMEMODEL_H
#define MARBLE_DECLARATIVE_STREETMAPTHEMEMODEL_H

#include <QtGui/QSortFilterProxyModel>

namespace Marble { class MapThemeManager; }

class StreetMapThemeModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY( int count READ count NOTIFY countChanged )

public:
    StreetMapThemeModel( Marble::MapThemeManager* themeManager );

    /** @todo FIXME https://bugreports.qt.nokia.com/browse/QTCOMPONENTS-1206 */
    int count();

public Q_SLOTS:
    QString name( const QString &id );

    int indexOf( const QString &id );

Q_SIGNALS:
    void countChanged();

protected:
    virtual bool filterAcceptsRow(int sourceRow,
                                  const QModelIndex &sourceParent) const;

private Q_SLOTS:
    void handleChangedThemes();

private:
    Marble::MapThemeManager* m_themeManager;

    QList<QString> m_streetMapThemeIds;
};

#endif
