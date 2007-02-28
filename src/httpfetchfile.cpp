#include "httpfetchfile.h"
#include "katlasdirs.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

HttpFetchFile::HttpFetchFile( QObject *parent ) : QObject( parent ) {
	m_pHttp = new QHttp(this);
	m_cachePath = KAtlasDirs::localDir() + "/cache/";

	if ( QDir( m_cachePath ).exists() == false ) 
		( QDir::root() ).mkpath( m_cachePath );
	// What if we don't succeed in creating the path?

	connect(m_pHttp, SIGNAL(requestFinished(int, bool)),
		this, SLOT(httpRequestFinished(int, bool)));
	connect(m_pHttp, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
		this, SLOT(checkResponseHeader(const QHttpResponseHeader &)));
}

void HttpFetchFile::downloadFile( QUrl url ){

	QString fileName = QFileInfo(url.path()).fileName();

	if (QFile::exists(fileName)) {
		qDebug( "File already exists" );
		emit downloadDone( fileName, false );
		return;
	}

	m_pFile = new QFile( m_cachePath + fileName);
	if (!m_pFile->open(QIODevice::WriteOnly)) {
		emit statusMessage( tr("Unable to save the file %1: %2.")
			.arg( m_cachePath + fileName).arg(m_pFile->errorString()) );
		delete m_pFile;
		m_pFile = 0;
		return;
	}

	m_pHttp->setHost( url.host(), url.port() != -1 ? url.port() : 80 );
	if ( !url.userName().isEmpty() )
		m_pHttp->setUser( url.userName(), url.password() );

	httpRequestAborted = false;
	httpGetId = m_pHttp->get( url.path(), m_pFile );
	m_pFileIdMap.insert( httpGetId, fileName );

	emit statusMessage( tr("Downloading data from Wikipedia ... ") );

};

void HttpFetchFile::cancelDownload()
{
	emit statusMessage( tr("Download cancelled ... ") );
	httpRequestAborted = true;
	m_pHttp->abort();
}

void HttpFetchFile::httpRequestFinished(int requestId, bool error)
{
	if (httpRequestAborted) {
		if (m_pFile) {
			m_pFile->close();
			m_pFile->remove();
			delete m_pFile;
			m_pFile = 0;
		}
		m_pFileIdMap.remove( requestId );
		return;
	}

	if (requestId != httpGetId)
		return;

	m_pFile->close();

	if ( error ) {
		m_pFile->remove();
		emit statusMessage( tr("Download failed: %1.")
			.arg(m_pHttp->errorString() ) );
	} else {
		emit statusMessage( tr("Download from Wikipedia finished ... ") );
	}

	emit downloadDone( m_pFileIdMap[requestId], error );
	m_pFileIdMap.remove( requestId );
	
	delete m_pFile;
	m_pFile = 0;
}

void HttpFetchFile::checkResponseHeader(const QHttpResponseHeader &responseHeader)
{
	if (responseHeader.statusCode() != 200) {
			qDebug() << QString(" response: %1").arg( responseHeader.statusCode() );
			emit statusMessage( tr("Download failed: %1.")
			.arg( responseHeader.reasonPhrase() ) );
			httpRequestAborted = true;
		m_pHttp->abort();
		return;
	}
}
#include "httpfetchfile.moc"
