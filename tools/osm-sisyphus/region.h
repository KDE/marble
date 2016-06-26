//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef REGION_H
#define REGION_H

#include <QString>
#include <QObject>
#include <QFileInfo>

class Region
{
    Q_PROPERTY(QString continent READ continent WRITE setContinent)
    Q_PROPERTY(QString country READ country WRITE setCountry)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString pbfFile READ pbfFile WRITE setPbfFile)
    Q_PROPERTY(QString id READ id WRITE setId)
    Q_PROPERTY(QString path READ path WRITE setPath)

public:
    Region();

    QString name() const;

    QString continent() const;

    QString country() const;

    void setName(const QString& arg);

    void setContinent(const QString& arg);

    void setCountry(const QString& arg);

    QString id() const;

    void setId(const QString &id);

    QString pbfFile() const;

    void setPbfFile(const QString &pbfFile);

    QString path() const;

    void setPath(const QString &path);

    bool operator==(const Region &other) const;

    static QString fileSize(const QFileInfo &file);

public Q_SLOTS:

private:
    QString m_name;
    QString m_continent;
    QString m_country;
    QString m_pbfFile;
    QString m_id;
    QString m_path;
};

#endif // REGION_H
