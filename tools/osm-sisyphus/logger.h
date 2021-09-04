//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class LoggerPrivate;

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger& instance();

    void setFilename(const QString &filename);

    void setStatus(const QString &id, const QString &name, const QString &status, const QString &message);
    
private:
    explicit Logger(QObject *parent = nullptr);

    ~Logger() override;

    LoggerPrivate* const d;
};

#endif // LOGGER_H
