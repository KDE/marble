#ifndef MAPTHEME_H
#define MAPTHEME_H

#include <QObject>

class QStandardItemModel;

typedef struct {
	bool enabled;
	QString type;
	QString name;
	QString dem;
} DgmlLayer;

class MapTheme : public QObject
{
    Q_OBJECT

public:
	MapTheme(QObject *parent = 0);
	int open( QString path );

	QString name() const { return m_name; }
	QString prefix() const { return m_prefix; }
	QString icon() const { return m_icon; }
	QString tilePrefix() const { return ( m_prefix + "/" + m_installmap.left( m_installmap.length()-4 ) ); }
	QString description() const { return m_description; }
	QString installMap() const { return m_installmap; }
	DgmlLayer bitmaplayer() const { return m_bitmaplayer; }
	DgmlLayer vectorlayer() const { return m_vectorlayer; }

	void detectMaxTileLevel();
	int maxTileLevel() const { return m_maxtilelevel; }

	static QStringList findMapThemes( const QString& );
	static QStandardItemModel* mapThemeModel( const QStringList& stringlist );
private:
	QString m_name;
	QString m_prefix;
	QString m_icon;
	QString m_tileprefix;
	QString m_description;
	QString m_installmap;
	DgmlLayer m_vectorlayer;
	DgmlLayer m_bitmaplayer;
	int m_maxtilelevel;
};

#endif // MAPTHEME_H
