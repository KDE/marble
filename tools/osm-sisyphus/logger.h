//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
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
    explicit Logger(QObject *parent = 0);

    ~Logger();

    LoggerPrivate* const d;
};

#endif // LOGGER_H
