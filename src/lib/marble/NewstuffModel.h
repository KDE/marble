// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_NEWSTUFFMODEL_H
#define MARBLE_NEWSTUFFMODEL_H

#include "marble_export.h"

#include <QAbstractListModel>

class QNetworkReply;

namespace Marble
{

class NewstuffModelPrivate;

class MARBLE_EXPORT NewstuffModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString provider READ provider WRITE setProvider NOTIFY providerChanged)
    Q_PROPERTY(QString targetDirectory READ targetDirectory WRITE setTargetDirectory NOTIFY targetDirectoryChanged)
    Q_PROPERTY(QString registryFile READ registryFile WRITE setRegistryFile NOTIFY registryFileChanged)

public:
    enum NewstuffRoles {
        Name = Qt::UserRole + 1,
        Author,
        License,
        Summary,
        Version,
        ReleaseDate,
        Preview,
        Payload,
        InstalledVersion,
        InstalledReleaseDate,
        InstalledFiles,
        IsInstalled,
        IsUpgradable,
        Category,
        IsTransitioning,
        PayloadSize,
        DownloadedSize
    };

    enum IdTag {
        PayloadTag,
        NameTag
    };

    /** Constructor */
    explicit NewstuffModel(QObject *parent = nullptr);

    /** Destructor */
    ~NewstuffModel() override;

    /** Overload of QAbstractListModel */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /** Overload of QAbstractListModel */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /** Overload of QAbstractListModel */
    QHash<int, QByteArray> roleNames() const override;

    /** @todo FIXME https://bugreports.qt-project.org/browse/QTCOMPONENTS-1206 */
    int count() const;

    /**
     * Add a newstuff provider
     */
    void setProvider(const QString &downloadUrl);

    QString provider() const;

    void setTargetDirectory(const QString &targetDirectory);

    QString targetDirectory() const;

    void setRegistryFile(const QString &registryFile, IdTag idTag = PayloadTag);

    QString registryFile() const;

public Q_SLOTS:
    void install(int index);

    void uninstall(int index);

    void cancel(int index);

Q_SIGNALS:
    void countChanged();

    void providerChanged();

    void targetDirectoryChanged();

    void registryFileChanged();

    void installationProgressed(int newstuffindex, qreal progress);

    void installationFinished(int newstuffindex);

    void installationFailed(int newstuffindex, const QString &error);

    void uninstallationFinished(int newstuffindex);

private Q_SLOTS:
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);

    void retrieveData();

    void mapInstalled(int exitStatus);

    void mapUninstalled();

    void contentsListed(int exitStatus);

private:
    NewstuffModelPrivate *const d;
    friend class NewstuffModelPrivate;

    Q_PRIVATE_SLOT(d, void handleProviderData(QNetworkReply *))
};

}

#endif // MARBLE_NEWSTUFFMODEL_H
