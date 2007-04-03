#ifndef MAPTHEME_H
#define MAPTHEME_H

#include <QtCore/QObject>
#include <QtGui/QColor>

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
	QColor labelColor() const { return m_labelcolor; }
	QString tilePrefix() const { return ( m_prefix ); }
	QString description() const { return m_description; }
	QString installMap() const { return m_installmap; }
	DgmlLayer bitmaplayer() const { return m_bitmaplayer; }
	DgmlLayer vectorlayer() const { return m_vectorlayer; }

	int maxTileLevel() const { return m_maxtilelevel; }

	static QStringList findMapThemes( const QString& );
	static QStandardItemModel* mapThemeModel( const QStringList& stringlist );
private:
	QString m_name;
	QString m_prefix;
	QString m_icon;
	QColor m_labelcolor;
	QString m_tileprefix;
	QString m_description;
	QString m_installmap;
	DgmlLayer m_vectorlayer;
	DgmlLayer m_bitmaplayer;
	int m_maxtilelevel;
};

#endif // MAPTHEME_H
