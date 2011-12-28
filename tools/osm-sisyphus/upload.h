#ifndef UPLOAD_H
#define UPLOAD_H

#include "region.h"

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QFileInfo>

class Upload : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool cacheDownloads READ cacheDownloads WRITE setCacheDownloads)
    Q_PROPERTY(bool uploadFiles READ uploadFiles WRITE setUploadFiles)

public:
    static Upload& instance();

    void uploadAndDelete(const Region &region, const QFileInfo &file);

    bool cacheDownloads() const;

    bool uploadFiles() const;

public Q_SLOTS:
    void setCacheDownloads(bool arg);

    void setUploadFiles(bool arg);

private:
    struct Package {
        Region region;
        QFileInfo file;

        bool operator==(const Package &other) const;
    };

    explicit Upload(QObject *parent = 0);

    void processQueue();

    bool upload(const Package &package);

    void deleteFile(const QFileInfo &file);

    QList<Package> m_queue;
    bool m_cacheDownloads;
    bool m_uploadFiles;
};

#endif // UPLOAD_H
