// SPDX-License-Identifier: LGPL-2.1-only WITH Qt-LGPL-exception-1.1 OR LGPL-3.0-only WITH Qt-LGPL-exception-1.1 OR GPL-3.0-only OR LicenseRef-Qt-Commercial
//
// SPDX-FileCopyrightText: 2015 The Qt Company Ltd. <https://www.qt.io/licensing/>
//
// This file is based on qzipwriter_p.h from Qt with the original license
// below, taken from
// https://code.qt.io/cgit/qt/qt.git/plain/src/gui/text/qzipwriter_p.h

#ifndef MARBLEZIPWRITER_H
#define MARBLEZIPWRITER_H
#ifndef QT_NO_TEXTODFWRITER

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QZipWriter class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <QFile>
#include <QString>

#include <marble_export.h>

namespace Marble
{

class MarbleZipWriterPrivate;

class MARBLE_EXPORT MarbleZipWriter
{
public:
    MarbleZipWriter(const QString &fileName, QIODevice::OpenMode mode = (QIODevice::WriteOnly | QIODevice::Truncate));

    explicit MarbleZipWriter(QIODevice *device);
    ~MarbleZipWriter();

    QIODevice *device() const;

    bool isWritable() const;
    bool exists() const;

    enum Status {
        NoError,
        FileWriteError,
        FileOpenError,
        FilePermissionsError,
        FileError
    };

    Status status() const;

    enum CompressionPolicy {
        AlwaysCompress,
        NeverCompress,
        AutoCompress
    };

    void setCompressionPolicy(CompressionPolicy policy);
    CompressionPolicy compressionPolicy() const;

    void setCreationPermissions(QFile::Permissions permissions);
    QFile::Permissions creationPermissions() const;

    void addFile(const QString &fileName, const QByteArray &data);

    void addFile(const QString &fileName, QIODevice *device);

    void addDirectory(const QString &dirName);

    void addSymLink(const QString &fileName, const QString &destination);

    void close();

private:
    MarbleZipWriterPrivate *d;
    Q_DISABLE_COPY(MarbleZipWriter)
};

}

#endif // QT_NO_TEXTODFWRITER
#endif // MARBLEZIPWRITER_H
