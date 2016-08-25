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
        QString urlPath = fileListUrl + QLatin1Char('/') + dirList.at(i);
        QDir columnDir = QDir(urlPath);
        QStringList files = columnDir.entryList(QStringList() << "*.jpg", QDir::Files);
        for (int j = 0; j < files.length(); ++j) {
            QString filePath = urlPath + QLatin1Char('/') + files.at(j);
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

//    int tileCountBathymetry = pow(2, m_tileLevel);
    int tileCount = pow(2, m_tileLevel);

    int xTile = filePath.section(QLatin1Char('/'), -2, -2).toUInt();
    int yTile = filePath.section(QLatin1Char('/'), -1).section(QLatin1Char('.'), 0, 0).toUInt();
    int x = m_bathymetry.width() * xTile / (double)tileCount;

    qreal lat_rad = atan(sinh(M_PI * (1 - 2 * yTile / (double)tileCount)));
    int y = (int)(m_bathymetry.height() * (-lat_rad + M_PI/2) / M_PI);

    int maskValueTopLeft = qRed(m_mask.pixel(2*x, 2*y));
    int maskValueTopRight = qRed(m_mask.pixel(2*x + 1, 2*y));
    int maskValueBottomLeft = qRed(m_mask.pixel(2*x, 2*y + 1));
    int maskValueBottomRight = qRed(m_mask.pixel(2*x + 1, 2*y + 1));

    if (maskValueTopLeft != 0 || maskValueTopRight != 0
            || maskValueBottomLeft != 0 || maskValueBottomRight != 0 ) { // for all areas which are not black
        QColor bathymetryColor = QColor(m_bathymetry.pixel(x, y));

        QImage tile(256, 256, QImage::Format_RGB32);
        tile.fill(bathymetryColor);

        QImage origTile;
        bool success = origTile.load(filePath);

        if (!success) qDebug() << "Loading tile failed: " << filePath;

        QPainter painter;
        painter.begin(&tile);
        qreal opacity;
        opacity = 1.0 - (double)(maskValueTopLeft)/255.0;
        painter.setOpacity(opacity);
        painter.drawImage(QRect(0, 0, 128, 128), origTile,
                          QRect(0, 0, 128, 128));
        opacity = 1.0 - (double)(maskValueTopRight)/255.0;
        painter.setOpacity(opacity);
        painter.drawImage(QRect(128, 0, 128, 128), origTile,
                          QRect(128, 0, 128, 128));
        opacity = 1.0 - (double)(maskValueBottomLeft)/255.0;
        painter.setOpacity(opacity);
        painter.drawImage(QRect(0, 128, 128, 128), origTile,
                          QRect(0, 128, 128, 128));
        opacity = 1.0 - (double)(maskValueBottomRight)/255.0;
        painter.setOpacity(opacity);
        painter.drawImage(QRect(128, 128, 128, 128), origTile,
                          QRect(128, 128, 128, 128));
        painter.end();

        QString modFilePath = filePath;
//        modFilePath = modFilePath.replace(".", "_mod.");

        tile.save(modFilePath, "JPG", 85);
        if (opacity > 0.0) {
            qDebug() << progress << filePath.section(QLatin1Char('/'), -2, -2) << filePath.section(QLatin1Char('/'), -1).section(QLatin1Char('.'), 0, 0);
            qDebug() << maskValueTopLeft << modFilePath;
        }
    }
    else {
//        qDebug() << maskValue;
    }
}
