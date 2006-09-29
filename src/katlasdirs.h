#ifndef KATLASDIRS_H
#define KATLASDIRS_H

#include <QApplication>
#include <QFile>
#include <QDir>

class KAtlasDirs
{
  public:
    static QString path( const QString& path ) 
	{ 
		QString fullpath = systemDir() + "/" + path;	// system path
		QString localpath = localDir() + "/" + path;	// local path
	
		if( QFile::exists( localpath ) )
			fullpath = localpath;

        	return QDir(fullpath).canonicalPath(); 
	}

    static QString systemDir() 
	{ 
          return QDir( qApp->applicationDirPath() 
#ifdef Q_OS_UNIX
	+ QLatin1String("/../data")
#endif
	  ).canonicalPath();
        }

    static QString localDir() 
	{ 
		return QString( QDir::homePath() + "/.globepedia/data");	// local path
	}

};

#endif // KATLASDIRS_H
