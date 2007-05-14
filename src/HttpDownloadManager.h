//
// C++ Interface: downloadmanager
//
// Description: HttpDownloadManager 

// The HttpDownloadManager manages http downloads.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution

#ifndef __MARBLE__HTTPDOWNLOADMANAGER_H
#define __MARBLE__HTTPDOWNLOADMANAGER_H

#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QQueue>
#include <QtCore/QList>

enum Priority { NoPriority, Low, Medium, High };
enum Status { NoStatus, Pending, Activated, Finished, Expired, Killed };

struct HttpJob {
    HttpJob()
    {
        status = NoStatus; priority = NoPriority;
    }
    virtual ~HttpJob(){ /* NONE */ }

    QString ServerUrl;
    QString SourceUrl;
    QDir TargetDir;

    Priority priority;
    Status status;
};

class HttpFetchFile;

/**
 *@author Torsten Rahn
 *
 * This class manages scheduled downloads. The downloadmanager offers
 * a maximum number of active jobs and a limit for pending jobs.
 * it also takes care that the job queue won't be polluted by 
 * jobs that timed out already.
 *
 */

class HttpDownloadManager : public QObject 
{
    Q_OBJECT
public:
    HttpDownloadManager();
    virtual ~HttpDownloadManager();

//    void addJob(HttpJob*){};
//    void killJob(HttpJob*){};
//    void killAllJobs(){};

    void setJobQueueLimit(int jobQueueLimit)
    {
        m_jobQueueLimit = jobQueueLimit;
    };

    void setActivatedJobsLimit(int activatedJobsLimit)
    {
        m_activatedJobsLimit = activatedJobsLimit;
    };

private:
    QQueue<HttpJob*> m_jobQueue;
    QList<HttpJob*> m_activatedJobList;

    HttpFetchFile  *m_fetchFile;

    int m_activatedJobsLimit;
    int m_jobQueueLimit;

//    QTimer* timer; // the timer needs to be set for each item in the activatedJobsList

private slots:

//    void addJob( const QString& ServerUrl, const QString& SourceUrl, const QString& TargetDir, Priority priority);
    void addJob( QString relativeUrl );

    void removeJob(HttpJob*);
    void activateJobs();
};

#endif // __MARBLE__HTTPDOWNLOADMANAGER_H
