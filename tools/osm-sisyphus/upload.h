#ifndef UPLOAD_H
#define UPLOAD_H

#include "region.h"

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QFileInfo>

class Upload : public QObject
{
    Q_OBJECT
public:
    static Upload& instance();

    void uploadAndDelete(const Region &region, const QFileInfo &file);

private:
    struct Package {
        Region region;
        QFileInfo file;

        bool operator==(const Package &other) const;
    };

    explicit Upload(QObject *parent = 0);

    void processQueue();

    void upload(const Package &package);

    void deleteFile(const QFileInfo &file);

    QList<Package> m_queue;
};

#endif // UPLOAD_H
