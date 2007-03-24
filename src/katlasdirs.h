#ifndef KATLASDIRS_H
#define KATLASDIRS_H

#include <QtGui/QApplication>
#include <QtCore/QFile>
#include <QtCore/QDir>

class KAtlasDirs
{
  public:
    static QString path( const QString& path ) 
	{ 
		QString fullpath = systemDir() + "/" + path;	// system path
		QString localpath = localDir() + "/" + path;	// local path
		QString unixpath = unixDir() + "/" + path;	// unix path
	
		if( QFile::exists( unixpath ) )
			fullpath = unixpath;
		else if( QFile::exists( localpath ) )
			fullpath = localpath;

        	return QDir(fullpath).canonicalPath(); 
	}

    static QString systemDir() 
	{ 
          return QDir( qApp->applicationDirPath() 
#if defined(Q_OS_MACX)
	+ QLatin1String("/Resources/data")
#elif defined(Q_OS_UNIX)
	+ QLatin1String("/../share/apps/marble/data")
#elif defined(Q_OS_WIN)
	+ QLatin1String("/data")
#endif
	  ).canonicalPath();
        }

    static QString localDir() 
	{ 
		return QString( QDir::homePath() + "/.katlas/data");	// local path
	}

    static QString unixDir()
        {
		return QString("/usr/share/marble/data");	// unix system path
	}

};

#endif // KATLASDIRS_H
