// SPDX-License-Identifier: LGPL-2.1-only WITH Qt-LGPL-exception-1.1 OR LGPL-3.0-only WITH Qt-LGPL-exception-1.1 OR GPL-3.0-only OR LicenseRef-Qt-Commercial
//
// SPDX-FileCopyrightText: 2015 The Qt Company Ltd. <https://www.qt.io/licensing/>
// This file is based on qzipreader_p.h from Qt with the original license
// below, taken from
// https://code.qt.io/cgit/qt/qt.git/plain/src/gui/text/qzipreader_p.h

#ifndef MARBLEZIPREADER_H
#define MARBLEZIPREADER_H

#ifndef QT_NO_TEXTODFWRITER

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QZipReader class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QDateTime>
#include <QFile>
#include <QString>

#include <marble_export.h>

namespace Marble
{

class MarbleZipReaderPrivate;

class MARBLE_EXPORT MarbleZipReader
{
public:
    MarbleZipReader(const QString &fileName, QIODevice::OpenMode mode = QIODevice::ReadOnly);

    explicit MarbleZipReader(QIODevice *device);
    ~MarbleZipReader();

    QIODevice *device() const;

    bool isReadable() const;
    bool exists() const;

    struct MARBLE_EXPORT FileInfo {
        FileInfo();
        FileInfo(const FileInfo &other);
        ~FileInfo();
        FileInfo &operator=(const FileInfo &other);
        bool isValid() const;
        QString filePath;
        uint isDir : 1;
        uint isFile : 1;
        uint isSymLink : 1;
        QFile::Permissions permissions;
        uint crc32;
        qint64 size;
        QDateTime lastModified;
        void *d;
    };

    QList<FileInfo> fileInfoList() const;
    int count() const;

    FileInfo entryInfoAt(int index) const;
    QByteArray fileData(const QString &fileName) const;
    bool extractAll(const QString &destinationDir) const;

    enum Status {
        NoError,
        FileReadError,
        FileOpenError,
        FilePermissionsError,
        FileError
    };

    Status status() const;

    void close();

private:
    MarbleZipReaderPrivate *d;
    Q_DISABLE_COPY(MarbleZipReader)
};

}

#endif // QT_NO_TEXTODFWRITER
#endif // MARBLEZIPREADER_H
