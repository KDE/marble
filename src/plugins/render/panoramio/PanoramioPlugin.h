//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Shashan Singh <shashank.personal@gmail.com>"
//

//
// This class is a test plugin.
//

#ifndef MARBLEPANORAMIOPLUGIN_H
#define MARBLEPANORAMIOPLUGIN_H
#define RADIANSTODEGREES 57.2957795

#include <QtCore/QObject>
#include "../lib/HttpDownloadManager.h"
#include "../lib/CacheStoragePolicy.h"
// #include "../lib/HttpJob.h"
#include "jsonparser.h"
#include "imagewidget.h"
#include "RenderPlugin.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
namespace Marble
{

/**
 * @short The class that specifies the a simple panormaio plugin
 *
 */

class PanoramioPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES(Marble::RenderPluginInterface)
    MARBLE_PLUGIN(PanoramioPlugin)

public:
    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon() const;


    void initialize();

    bool isInitialized() const;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0);

public slots:
    void slotDownloadImage(QString , QString);   //completed download of json reply fom panoramio
    void slotAppendImageToList(QString , QString);   //completed download of image
signals:
    void statusMessageForImageDownloadingProcess(QString);
private:
    CacheStoragePolicy *m_storagePolicy;
    HttpDownloadManager *m_downloadManager;
    jsonParser panoramioJsonParser;
    int decimalToSexagecimal();//convert decimal to DMS system
    void downloadPanoramio(int, int, qreal, qreal, qreal, qreal);
    QList <panoramioDataStructure> parsedData;
    panoramioDataStructure temp;
    QPixmap tempImage;
    int flag;//this flag is one when globe has an Image  (downloaded or already there in cache)
    int numberOfImagesToShow;//this factor stires how many are to be downloaded and shown on the globe
    QList<imageWidget*>images;//these widgets are supposed to show draw images and take click events 
};

}

#endif
