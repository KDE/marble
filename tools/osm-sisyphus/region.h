//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef REGION_H
#define REGION_H

#include <QString>
#include <QObject>

class Region
{
    Q_PROPERTY(QString continent READ continent WRITE setContinent)
    Q_PROPERTY(QString country READ country WRITE setCountry)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString pbfFile READ pbfFile WTITE setPbfFile)
    Q_PROPERTY(QString id READ id WTITE setId)
    Q_PROPERTY(QString path READ path WTITE setPath)

public:
    Region();

    QString name() const;

    QString continent() const;

    QString country() const;

    void setName(QString arg);

    void setContinent(QString arg);

    void setCountry(QString arg);

    QString id() const;

    void setId(const QString &id);

    QString pbfFile() const;

    void setPbfFile(const QString &pbfFile);

    QString path() const;

    void setPath(const QString &path);

    bool operator==(const Region &other) const;

public slots:

private:
    QString m_name;
    QString m_continent;
    QString m_country;
    QString m_pbfFile;
    QString m_id;
    QString m_path;
};

#endif // REGION_H
