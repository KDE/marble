//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_SPEAKERSMODEL_H
#define MARBLE_SPEAKERSMODEL_H

#include "marble_export.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QFileInfo>

namespace Marble
{

class SpeakersModelPrivate;

class MARBLE_EXPORT SpeakersModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum SpeakersModelRoles {
        Name = Qt::UserRole + 1,
        Path,
        IsLocal,
        IsRemote
    };

    /** Constructor */
    explicit SpeakersModel( QObject *parent = 0 );

    /** Destructor */
    ~SpeakersModel();

    /** Overload of QAbstractListModel */
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const;

    /** Overload of QAbstractListModel */
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    /** @todo FIXME https://bugreports.qt.nokia.com/browse/QTCOMPONENTS-1206 */
    int count();

public Q_SLOTS:
    int indexOf( const QString &name );

    QString path( int index );

    void install( int index );

    bool isLocal( int index ) const;

    bool isRemote( int index ) const;

Q_SIGNALS:
    void countChanged();

    void installationFinished( int index );

private:
    SpeakersModelPrivate* const d;
    friend class SpeakersModelPrivate;

    Q_PRIVATE_SLOT( d, void fillModel() )

    Q_PRIVATE_SLOT( d, void handleInstallation( int row ) )
};

}

#endif // MARBLE_SPEAKERSMODEL_H
