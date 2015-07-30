//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

#include "SearchBackend.h"
#include "MarblePlacemarkModel.h"
#include "MarbleModel.h"

using namespace Marble;


SearchBarBackend::SearchBarBackend(QObject *parent) :
    QObject(parent),
    m_searchManager( nullptr ),
    m_marbleQuickItem( nullptr ),
    m_placemarkModel( nullptr ),
    m_completer( nullptr ),
    m_completionModel( new MarblePlacemarkModel ),
    m_completionContainer( new QVector<GeoDataPlacemark*>() )
{
    m_model.setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    m_model.sort(0);
    m_model.setDynamicSortFilter(true);
    m_completionModel->setPlacemarkContainer(m_completionContainer);
}

void SearchBarBackend::search(const QString &place)
{
    if (m_marbleQuickItem)
    {
        m_searchManager->searchPlacemarks(place);
    }
}

void SearchBarBackend::setCompletionPrefix(const QString &prefix)
{
    if( m_completer != nullptr && m_completer->completionPrefix() != prefix ) {
        m_completionModel->removePlacemarks(QString("Completion model"), 0, m_completionModel->rowCount());
        m_completionContainer->clear();
        m_completer->setCompletionPrefix(prefix);
        if( prefix.isEmpty() ) {
            emit completionModelChanged(m_completionModel);
            return;
        }
        QVector<GeoDataPlacemark*> *container = new QVector<GeoDataPlacemark*>();
        QAbstractProxyModel *model = qobject_cast<QAbstractProxyModel*>(m_completer->completionModel());
        for( int i = 0; i<m_completer->completionModel()->rowCount(); ++i ) {
            QModelIndex index = model->mapToSource(model->index(i,0));
            QVariant data = m_marbleQuickItem->model()->placemarkModel()->data(index, MarblePlacemarkModel::ObjectPointerRole);
            GeoDataPlacemark *placemark = placemarkFromQVariant(data);
            if( placemark != nullptr ) {
                container->append(placemark);
            }
        }
        m_completionModel->setPlacemarkContainer(container);
        m_completionModel->addPlacemarks(0, container->size());
        delete m_completionContainer;
        m_completionContainer = container;
        emit completionModelChanged(m_completionModel);
    }
}

QObject * SearchBarBackend::marbleQuickItem()
{
    return m_marbleQuickItem;
}

MarblePlacemarkModel *SearchBarBackend::completionModel()
{
    return m_completionModel;
}

const QObject * SearchBarBackend::marbleQuickItem() const
{
    return m_marbleQuickItem;
}

void SearchBarBackend::updateMap(int placemarkIndex)
{
    QVariant data = m_placemarkModel->data(m_placemarkModel->index(placemarkIndex), MarblePlacemarkModel::ObjectPointerRole);
    GeoDataPlacemark *placemark = placemarkFromQVariant(data);
    if( placemark == nullptr ) {
        return;
    }
    m_marbleQuickItem->centerOn(*placemark, true);
}

void SearchBarBackend::setMarbleQuickItem(QObject *marbleQuickItem)
{
    MarbleQuickItem * item = qobject_cast<MarbleQuickItem*>(marbleQuickItem);
    if (m_marbleQuickItem == item)
    {
        return;
    }

    if (item)
    {
        delete m_searchManager;
        delete m_completer;
        m_marbleQuickItem = item;
        m_searchManager = new SearchRunnerManager(m_marbleQuickItem->model(), this);

        connect(m_searchManager, SIGNAL(searchResultChanged(QAbstractItemModel*)),
                this, SLOT(searchFinished(QAbstractItemModel*)));

        m_completer = new QCompleter();
        m_completer->setModel(m_marbleQuickItem->model()->placemarkModel());
        m_completer->setCompletionRole(Qt::DisplayRole);
        m_completer->setCaseSensitivity(Qt::CaseInsensitive);

        emit marbleQuickItemChanged(marbleQuickItem);
    }
}

void SearchBarBackend::searchFinished(QAbstractItemModel *result)
{
    m_placemarkModel = qobject_cast<MarblePlacemarkModel*>(result);
    emit updateSearchResults(m_placemarkModel);
}

GeoDataPlacemark *SearchBarBackend::placemarkFromQVariant(const QVariant &data) const
{
    if( !data.isValid() ) {
        return nullptr;
    }
    GeoDataObject *object = qvariant_cast<GeoDataObject*>( data );
    if( object == nullptr ) {
        return nullptr;
    }
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( object );
    if( placemark == nullptr ) {
        return nullptr;
    }
    return placemark;
}

#include "moc_SearchBackend.cpp"
