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
#include "MarbleAbstractLayer.h"

/**
 * @short The class that specifies the a simple panormaio plugin
 *
 */

class PanoramioPlugin : public MarbleAbstractLayer
{
        Q_OBJECT
        Q_INTERFACES ( MarbleLayerInterface )

    public:
        QStringList backendTypes() const;

        QString renderPolicy() const;

        QStringList renderPosition() const;

        QString name() const;

        QString guiString() const;

        QString nameId() const;

        QString description() const;

        QIcon icon () const;


        void initialize ();

        bool isInitialized () const;

        bool render ( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );

    public slots:
        void slotJsonDownloadComplete ( QString , QString );//completed download of json reply fom panoramio
        void slotImageDownloadComplete ( QString , QString );//completed download of image
    signals:
	void statusMessageForImageDownloadingProcess(QString);
    private:
        CacheStoragePolicy *m_storagePolicy;
        HttpDownloadManager *m_downloadManager;
        jsonParser panoramioJsonParser;
        int decimalToSexagecimal();//convert decimal to DMS system
        void downloadPanoramio ( int,int,double,double,double,double);
        QList <QPixmap > imagesWeHave;//this list will hold pointers to pixmaps we have downloaded
        QList <panoramioDataStructure> parsedData;
        panoramioDataStructure temp;
        QPixmap tempImage;
        int flag;//this flag is one when globe has an Image  (downloaded or already there in cache)
//         HttpJob *job;
        int numberOfImagesToShow;//this factor stires how many are to be downloaded and shown on the globe
};

#endif
