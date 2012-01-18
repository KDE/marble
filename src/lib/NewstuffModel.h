//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_NEWSTUFFMODEL_H
#define MARBLE_NEWSTUFFMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QFileInfo>

class QNetworkReply;

namespace Marble
{

class NewstuffModelPrivate;

class NewstuffModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString provider READ provider WRITE setProvider NOTIFY providerChanged)
    Q_PROPERTY(QString targetDirectory READ targetDirectory WRITE setTargetDirectory NOTIFY targetDirectoryChanged)

public:
    enum NewstuffRoles {
        Name = Qt::UserRole + 1,
        Author,
        Licence,
        Summary,
        Version,
        ReleaseDate,
        Preview,
        Payload
    };

    /** Constructor */
    explicit NewstuffModel( QObject *parent = 0 );

    /** Destructor */
    ~NewstuffModel();

    /** Overload of QAbstractListModel */
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const;

    /** Overload of QAbstractListModel */
    QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    /** @todo FIXME https://bugreports.qt.nokia.com/browse/QTCOMPONENTS-1206 */
    int count();

    /**
      * Add a newstuff provider
      */
    void setProvider( const QString &downloadUrl );

    QString provider() const;

    void setTargetDirectory( const QString &targetDirectory );

    QString targetDirectory() const;

public Q_SLOTS:
    void install( int index );

Q_SIGNALS:
    void countChanged();

    void providerChanged();

    void targetDirectoryChanged();

    void installationProgressed( int index, qreal progress );

    void installationFinished( int index );

    void installationFailed( int index, const QString &error );

private Q_SLOTS:
    void updateProgress( qint64 bytesReceived, qint64 bytesTotal );

    void retrieveData();

    void mapInstalled( int exitStatus );

private:
    NewstuffModelPrivate* const d;
    friend class NewstuffModelPrivate;

    Q_PRIVATE_SLOT( d, void handleProviderData( QNetworkReply* ) )
};

}

#endif // MARBLE_NEWSTUFFMODEL_H
