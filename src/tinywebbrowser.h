//
// C++ Interface: tinywebbrowser
//
// Description: tinywebbrowser

// The Tiny Web Browser is a web browser based on QTextBrowser.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#ifndef TINYWEBBROWSER_H
#define TINYWEBBROWSER_H

#include <QtGui/QTextBrowser>

/**
@author Torsten Rahn
*/

class HttpFetchFile;

class TinyWebBrowser : public QTextBrowser {

Q_OBJECT

public:
	TinyWebBrowser( QWidget* parent );

public slots:
	void setSource( QUrl url );
	void slotDownloadFinished( QString, bool );

signals:
	void statusMessage( QString );

protected:
	HttpFetchFile * m_pFetchFile;
	virtual QVariant loadResource ( int type, const QUrl & name );
	QString m_source;
	QList<QUrl> m_urlList;
};

#endif // TINYWEBBROWSER_H
