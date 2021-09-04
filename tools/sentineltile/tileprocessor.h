//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Torsten Rahn <tackat@kde.org>
//

#ifndef TILEPROCESSOR_H
#define TILEPROCESSOR_H

#include <QObject>
#include <QColor>
#include <QImage>


class TileProcessor : public QObject
{
    Q_OBJECT
public:
    explicit TileProcessor(QObject *parent = nullptr);

public:

    void parseFileList(const QString& fileListUrl);

    void loadReferenceImages(const QString& maskPath, const QString& bathymetryPath);

    void process();

private:
    void colorForFile(const QString& filePath);

    QStringList m_fileList;

    QImage m_mask;
    QImage m_bathymetry;

    int m_tileLevel;
    static int progress;

};

#endif // TILEPROCESSOR_H
