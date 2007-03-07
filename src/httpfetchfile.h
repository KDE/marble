//
// C++ Interface: httpfetch
//
// Description: httpfetch

// The HttpFetchFile class downloads files.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution

#ifndef HTTPFETCHFILE_H
#define HTTPFETCHFILE_H

#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtNetwork/QHttpResponseHeader>

/**
@author Torsten Rahn
*/

class HttpFetchFile : public QObject {

Q_OBJECT

public:
	HttpFetchFile( QObject* parent = 0 );

public slots:
	void downloadFile( QUrl );
	void cancelDownload();
	void httpRequestFinished(int requestId, bool error);
	void checkResponseHeader(const QHttpResponseHeader &responseHeader);

signals:
	void downloadDone( QString, bool );
	void statusMessage( QString );

protected:
	QHttp * m_pHttp;
	QFile * m_pFile;
	int httpGetId;
	bool httpRequestAborted;
	QMap <int, QString> m_pFileIdMap;

	QString m_cachePath;
};

#endif // HTTPFETCHFILE_H
