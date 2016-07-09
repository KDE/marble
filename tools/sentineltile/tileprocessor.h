//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016 Torsten Rahn <tackat@kde.org>
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
    explicit TileProcessor(QObject *parent = 0);

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
