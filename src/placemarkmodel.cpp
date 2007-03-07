#include "placemarkmodel.h"

#include <cmath>
#include "katlasdirs.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QModelIndex>
#include <QtGui/QIcon>
#include <QtCore/QDebug>
#include <QtCore/QTime>

#ifdef KDEBUILD
#include "placemarkmodel.moc"
#endif

PlaceMarkModel::PlaceMarkModel(QObject *parent) : QAbstractListModel(parent){
}

PlaceMarkModel::~PlaceMarkModel(){
}

int PlaceMarkModel::rowCount(const QModelIndex &parent) const {
	return m_placemarkindex.size();
}

int PlaceMarkModel::columnCount(const QModelIndex &parent) const {
	return 1;
}

PlaceMark* PlaceMarkModel::placeMark(const QModelIndex &index) const {
	if (!index.isValid())
		return 0;

	if (index.row() >= m_placemarkindex.size())
		return 0;

	return m_placemarkindex.at(index.row());
}

QVariant PlaceMarkModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid())
	return QVariant();

	if (index.row() >= m_placemarkindex.size())
		return QVariant();

	if (role == Qt::DisplayRole){
		return m_placemarkindex.at(index.row())->name();
	}
	if (role == Qt::DecorationRole){
		return m_placemarkindex.at(index.row())->symbolPixmap();
	}
	else
		return QVariant();
}

QVariant PlaceMarkModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

void PlaceMarkModel::setContainer(PlaceContainer* container){

	PlaceContainer::const_iterator it;

	for ( it=container->constBegin(); it != container->constEnd(); it++ ){ // STL iterators
		m_placemarkindex << *it;
	}
	qStableSort( m_placemarkindex.begin(), m_placemarkindex.end(), nameSort );
}
