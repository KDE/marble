//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016 Torsten Rahn <tackat@kde.org>
//

#include "tileprocessor.h"

#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#include <cmath>

int TileProcessor::progress = 0;

TileProcessor::TileProcessor(QObject *parent) : QObject(parent),
    m_tileLevel(14)
{

}

void TileProcessor::parseFileList(const QString& fileListUrl) {

    m_fileList.clear();

    QDir rootDir(fileListUrl);

    QStringList dirList = rootDir.entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < dirList.length(); ++i) {
        QString urlPath = fileListUrl + "/" + dirList.at(i);
        QDir columnDir = QDir(urlPath);
        QStringList files = columnDir.entryList(QStringList() << "*.jpg", QDir::Files);
        for (int j = 0; j < files.length(); ++j) {
            QString filePath = urlPath + "/" + files.at(j);
            m_fileList << filePath;
        }
    }
    qDebug() << "Total file count: " << m_fileList.length();
}

void TileProcessor::loadReferenceImages(const QString& maskPath, const QString& bathymetryPath) {
    qDebug() << "Loading mask from" << maskPath;
    bool success = m_mask.load(maskPath);
    if (!success) qDebug() << "Loading mask failed: " << maskPath;
    qDebug() << "Loading bathymetry from" << bathymetryPath;
    success = m_bathymetry.load(bathymetryPath);
    if (!success) qDebug() << "Loading bathymetry failed: " << bathymetryPath;
    qDebug() << "Reference images loaded.";

    if (m_mask.width() != m_bathymetry.width() || m_mask.height() != m_bathymetry.height()) {
        qDebug() << "Mask and bathymetry geometries don't match:";
        qDebug() << "Mask: " << m_mask.width() << "x" << m_mask.height();
        qDebug() << "Bathymetry: " << m_bathymetry.width() << "x" << m_bathymetry.height();
    }
}

void TileProcessor::process() {
    for (int i = 0; i < m_fileList.length(); ++i) {
        QFileInfo fileInfo(m_fileList.at(i));
        if (fileInfo.isFile()) {
            colorForFile(m_fileList.at(i));
        }
    }
}

void TileProcessor::colorForFile(const QString& filePath){
    ++progress;

    int tileCount = pow(2, m_tileLevel);

    int x = m_mask.width() * (filePath.section('/', -2, -2).toDouble()/ (double)tileCount);

    int ypos = filePath.section('/', -1).section(".",0, 0).toDouble();

    qreal lat_rad = atan(sinh(M_PI * (1 - 2 * ypos / (double)tileCount)));
    qreal yreal = m_mask.height() * (-lat_rad + M_PI/2) / M_PI;

    int y = (int)yreal;

    int maskValue = qRed(m_mask.pixel(x, y));

    if (maskValue != 0) { // for all areas which are not black
        QColor bathymetryColor = QColor(m_bathymetry.pixel(x, y));

        QImage tile(256, 256, QImage::Format_RGB32);
        tile.fill(bathymetryColor);

        QImage origTile;
        bool success = origTile.load(filePath);

        if (!success) qDebug() << "Loading tile failed: " << filePath;

        QPainter painter;
        painter.begin(&tile);
        qreal opacity = 1.0 - (double)(maskValue)/255.0;
        painter.setOpacity(opacity);
        painter.drawImage(0, 0, origTile);
        painter.end();

        QString modFilePath = filePath;
//        modFilePath = modFilePath.replace(".", "_mod.");

        tile.save(modFilePath, "JPG", 85);
        if (opacity > 0.0) {
            qDebug() << progress << filePath.section('/', -2, -2) << filePath.section('/', -1).section(".",0, 0);
            qDebug() << maskValue << modFilePath;
        }
    }
    else {
//        qDebug() << maskValue;
    }
}
