//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "logger.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class LoggerPrivate
{
public:
    QSqlDatabase m_database;

    LoggerPrivate();

    void initializeDatabase();
};

LoggerPrivate::LoggerPrivate()
{
    m_database = QSqlDatabase::addDatabase( "QSQLITE" );
}

void LoggerPrivate::initializeDatabase()
{
    QSqlQuery createJobsTable( "CREATE TABLE IF NOT EXISTS jobs (id VARCHAR(255) PRIMARY KEY, name TEXT, status TEXT, description TEXT, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);" );
    if ( createJobsTable.lastError().isValid() ) {
        qDebug() << "Error when executing query" << createJobsTable.lastQuery();
        qDebug() << "Sql reports" << createJobsTable.lastError();
    }
}

Logger::Logger(QObject *parent) :
    QObject(parent), d(new LoggerPrivate)
{
}

Logger::~Logger()
{
    delete d;
}

Logger &Logger::instance()
{
    static Logger m_instance;
    return m_instance;
}

void Logger::setFilename(const QString &filename)
{
    d->m_database.setDatabaseName( filename );
    if ( !d->m_database.open() ) {
        qDebug() << "Failed to connect to database " << filename;
    }

    d->initializeDatabase();
}

void Logger::setStatus(const QString &id, const QString &name, const QString &status, const QString &message)
{
    QSqlQuery deleteJob( QString("DELETE FROM jobs WHERE id='%1';").arg(id) );
    if ( deleteJob.lastError().isValid() ) {
        qDebug() << "Error when executing query" << deleteJob.lastQuery();
        qDebug() << "Sql reports" << deleteJob.lastError();
    } else {
        QSqlQuery createStatus;
        createStatus.prepare("INSERT INTO jobs (id, name, status, description) VALUES (:job, :name, :status, :message);");
        createStatus.bindValue(":job", id);
        createStatus.bindValue(":name", name);
        createStatus.bindValue(":status", status);
        createStatus.bindValue(":message", message);
        if ( !createStatus.exec() ) {
            qDebug() << "Error when executing query" << createStatus.lastQuery();
            qDebug() << "Sql reports" << createStatus.lastError();
        }
    }
}

#include "moc_logger.cpp"
