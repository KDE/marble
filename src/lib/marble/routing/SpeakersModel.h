// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_SPEAKERSMODEL_H
#define MARBLE_SPEAKERSMODEL_H

#include "marble_export.h"

#include <QAbstractListModel>

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
    explicit SpeakersModel(QObject *parent = nullptr);

    /** Destructor */
    ~SpeakersModel() override;

    /** Overload of QAbstractListModel */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /** Overload of QAbstractListModel */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /** Overload of QAbstractListModel */
    QHash<int, QByteArray> roleNames() const override;

    /** @todo FIXME https://bugreports.qt-project.org/browse/QTCOMPONENTS-1206 */
    int count() const;

public Q_SLOTS:
    int indexOf(const QString &name);

    QString path(int index);

    void install(int index);

    bool isLocal(int index) const;

    bool isRemote(int index) const;

Q_SIGNALS:
    void countChanged();

    void installationProgressed(int newstuffindex, qreal progress);

    void installationFinished(int index);

private:
    SpeakersModelPrivate *const d;
    friend class SpeakersModelPrivate;

    Q_PRIVATE_SLOT(d, void fillModel())

    Q_PRIVATE_SLOT(d, void handleInstallationProgress(int row, qreal progress))

    Q_PRIVATE_SLOT(d, void handleInstallation(int row))
};

}

#endif // MARBLE_SPEAKERSMODEL_H
