//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Mikhail Ivchenko <ematirov@gmail.com>


#include "PlacemarkEditHeader.h"
#include "ui_PlacemarkEditHeader.h"

#include <QWidget>
#include <QFileDialog>
#include "MarbleDebug.h"

#include "MarbleGlobal.h"


namespace Marble
{

class PlacemarkEditHeaderPrivate : public Ui::PlacemarkEditHeaderPrivate
{

public:
    explicit PlacemarkEditHeaderPrivate(PlacemarkEditHeader *_q);
    ~PlacemarkEditHeaderPrivate();
    void init(QWidget* parent);
    void setNotation(GeoDataCoordinates::Notation notation);
    GeoDataCoordinates::Notation notation() const;
    void setName(const QString &name);
    QString name() const;
    void setIconLink(const QString &iconLink);
    QString iconLink() const;
    qreal longitude() const;
    void setLongitude(qreal longitude);
    qreal latitude() const;
    void setLatitude(qreal latitude);
    QString id() const;
    QString setId( const QString &id, bool isNew = false );
    QStringList idFilter() const;
    void setIdFilter( const QStringList &filter );
    bool isIdVisible() const;
    void setIdVisible( bool visible );
    bool isIdValid() const;
    void loadIconFile();
    bool positionVisible() const;
    void setPositionVisible( bool visible );
    QString targetId() const;
    void setTargetId( const QString &targetId );
    QStringList targetIdList() const;
    void setTargetIdList( const QStringList &targetIdList );
    bool isTargetIdVisible() const;
    void setTargetIdVisible( bool visible );
    void updateValues();
    void selectAll();
private:
    PlacemarkEditHeader* const q;
    bool m_positionVisible;
    QStringList m_idFilter;
    bool m_idWasEdited;
    bool m_idValid;
    QString m_idString;
    QString m_targetIdString;
};

PlacemarkEditHeaderPrivate::PlacemarkEditHeaderPrivate(PlacemarkEditHeader* _q)
    :q(_q),
    m_positionVisible(true),
    m_idWasEdited( false )
{}

PlacemarkEditHeaderPrivate::~PlacemarkEditHeaderPrivate()
{}

void PlacemarkEditHeaderPrivate::init(QWidget* parent)
{
    setupUi(parent);
    m_longitude->setDimension( Longitude );
    m_latitude->setDimension( Latitude );
    m_targetId->setVisible(false);
    targetIdLabel->setVisible(false);
    setNotation(GeoDataCoordinates::DMS);
    QObject::connect( iconLinkButton, SIGNAL(clicked()), q, SLOT(loadIconFile()) );
}

void PlacemarkEditHeaderPrivate::setNotation(GeoDataCoordinates::Notation notation)
{
    m_longitude->setNotation(notation);
    m_latitude->setNotation(notation);
}

GeoDataCoordinates::Notation PlacemarkEditHeaderPrivate::notation() const
{
    return m_longitude->notation();
}

void PlacemarkEditHeaderPrivate::setName(const QString &name)
{
    m_name->setText(name);
    if( !m_idWasEdited ) {
        setId( name );
    }
}

QString PlacemarkEditHeaderPrivate::name() const
{
    return m_name->text();
}

void PlacemarkEditHeaderPrivate::setIconLink(const QString &iconLink)
{
    QPixmap pixmap(iconLink);
    if( !pixmap.isNull() ) {
        QIcon icon(pixmap);
        m_iconLink->setText(iconLink);
        iconLinkButton->setText(QString());
        iconLinkButton->setIcon( icon );
    }
}

QString PlacemarkEditHeaderPrivate::iconLink() const
{
    return m_iconLink->text();
}

qreal PlacemarkEditHeaderPrivate::longitude() const
{
    return m_longitude->value();
}

void PlacemarkEditHeaderPrivate::setLongitude(qreal longitude)
{
    m_longitude->setValue(longitude);
}

qreal PlacemarkEditHeaderPrivate::latitude() const
{
    return m_latitude->value();
}

void PlacemarkEditHeaderPrivate::setLatitude(qreal latitude)
{
    m_latitude->setValue(latitude);
}

QString PlacemarkEditHeaderPrivate::id() const
{
    return m_id->text();
}

QString PlacemarkEditHeaderPrivate::setId( const QString &id, bool isNew )
{
    QString newId = id.toLower().replace(QLatin1Char(' '), QLatin1Char('_'));
    if( newId == m_idString && !isNew ) {
        return newId;
    }
    if( !isNew ) {
        m_idFilter.removeOne( m_idString );
    }
    if( m_idFilter.contains( newId ) && m_id->hasFocus() ) {
        m_idValid = false;
    } else {
        m_idValid = true;
        if ( m_idFilter.contains( newId ) ) {
            QStringList filter;
            foreach ( const QString &filterString , m_idFilter ) {
                if( filterString.startsWith( newId ) ) {
                    filter.append( filterString );
                }
            }
            int i = 2;
            while( filter.contains( newId + QString::number( i ) ) ) {
                i++;
            }
            newId += QString::number( i );
        }
    }
    m_idString = newId;
    m_idFilter.append( newId );
    m_id->setText( newId );
    return newId;
}

QStringList PlacemarkEditHeaderPrivate::idFilter() const
{
    return m_idFilter;
}

void PlacemarkEditHeaderPrivate::setIdFilter(const QStringList &filter)
{
    m_idFilter = filter;
    setId( id(), true );
}

bool PlacemarkEditHeaderPrivate::isIdVisible() const
{
    return m_id->isVisible() && idLabel->isVisible();
}

void PlacemarkEditHeaderPrivate::setIdVisible(bool visible)
{
    m_id->setVisible( visible );
    idLabel->setVisible( visible );
}

bool PlacemarkEditHeaderPrivate::isIdValid() const
{
    return m_idValid;
}

void PlacemarkEditHeaderPrivate::loadIconFile()
{
    const QString filename = QFileDialog::getOpenFileName( q,
                                                           QObject::tr( "Open File" ),
                                                           QString(),
                                                           QObject::tr( "All Supported Files (*.png)" ) );
    if ( filename.isNull() ) {
        return;
    }

    setIconLink(filename);
    iconLinkButton->setIcon( QIcon(filename) );
}

bool PlacemarkEditHeaderPrivate::positionVisible() const
{
    return m_positionVisible;
}

void PlacemarkEditHeaderPrivate::setPositionVisible(bool visible)
{
    m_longitude->setVisible(visible);
    m_latitude->setVisible(visible);
    positionLabel->setVisible(visible);
    m_positionVisible = visible;
}

QString PlacemarkEditHeaderPrivate::targetId() const
{
    return m_targetId->currentText();
}

void PlacemarkEditHeaderPrivate::setTargetId(const QString &targetId)
{
    m_targetId->setCurrentIndex( m_targetId->findText( targetId ) );
    m_targetIdString = targetId;
}

QStringList PlacemarkEditHeaderPrivate::targetIdList() const
{
    QStringList result;
    for( int i = 0; i < m_targetId->count(); ++i ) {
        result.append( m_targetId->itemText( i ) );
    }
    return result;
}

void PlacemarkEditHeaderPrivate::setTargetIdList(const QStringList &targetIdList)
{
    QString current;
    if( m_targetId->currentIndex() != -1 ) {
        current = m_targetId->currentText();
    } else {
        current = m_targetIdString;
    }
    m_targetId->clear();
    m_targetId->addItems( targetIdList );
    setTargetId( current );
}

bool PlacemarkEditHeaderPrivate::isTargetIdVisible() const
{
    return m_targetId->isVisible() && targetIdLabel->isVisible();
}

void PlacemarkEditHeaderPrivate::setTargetIdVisible(bool visible)
{
    m_targetId->setVisible( visible );
    targetIdLabel->setVisible( visible );
}

void PlacemarkEditHeaderPrivate::updateValues()
{
    if( m_idString != id() ) {
        setId( id() );
        m_idWasEdited = true;
    } else if( !m_idWasEdited && isIdVisible() ){
        setId( name() );
    }
}

void PlacemarkEditHeaderPrivate::selectAll()
{
    m_name->selectAll();
}

}


using namespace Marble;

PlacemarkEditHeader::PlacemarkEditHeader(QWidget *parent, GeoDataCoordinates::Notation notation,
                                         const QString& name,
                                         const QString& iconLink,
                                         const QString& id,
                                         const QStringList &idFilter)
    : QWidget( parent ), d(new PlacemarkEditHeaderPrivate(this))
{
    d->init(this);
    d->setNotation(notation);
    d->setName(name);
    d->setIconLink(iconLink);
    d->setIdFilter(idFilter);
    d->setId( id.isEmpty() ? d->name() : id );
    connect(d->m_longitude,   SIGNAL(valueChanged(qreal)),    this, SLOT(updateValues()));
    connect(d->m_latitude,  SIGNAL(valueChanged(qreal)),    this, SLOT(updateValues()));
    connect(d->m_name,  SIGNAL(textChanged(QString)),    this, SLOT(updateValues()));
    connect(d->m_iconLink,  SIGNAL(textChanged(QString)),    this, SLOT(updateValues()));
    connect(d->m_id,  SIGNAL(textChanged(QString)),    this, SLOT(updateValues()));
}

PlacemarkEditHeader::~PlacemarkEditHeader()
{
    delete d;
}

QString PlacemarkEditHeader::name() const
{
    return d->name();
}

QString PlacemarkEditHeader::iconLink() const
{
    return d->iconLink();
}

qreal PlacemarkEditHeader::latitude() const
{
    return d->latitude();
}

qreal PlacemarkEditHeader::longitude() const
{
    return d->longitude();
}

GeoDataCoordinates::Notation PlacemarkEditHeader::notation() const
{
    return d->notation();
}

bool PlacemarkEditHeader::positionVisible() const
{
    return d->positionVisible();
}

QString PlacemarkEditHeader::id() const
{
    return d->id();
}

QStringList PlacemarkEditHeader::idFilter() const
{
    return d->idFilter();
}

bool PlacemarkEditHeader::isIdVisible() const
{
    return d->isIdVisible();
}

bool PlacemarkEditHeader::isIdValid() const
{
    return d->isIdValid();
}

QString PlacemarkEditHeader::targetId() const
{
    return d->targetId();
}

QStringList PlacemarkEditHeader::targetIdList() const
{
    return d->targetIdList();
}

bool PlacemarkEditHeader::isTargetIdVisible() const
{
    return d->isTargetIdVisible();
}

void PlacemarkEditHeader::setName(const QString &name)
{
    d->setName(name);
}

void PlacemarkEditHeader::setIconLink(const QString &iconLink)
{
    d->setIconLink(iconLink);
}

void PlacemarkEditHeader::setLatitude(qreal latitude)
{
    d->setLatitude(latitude);
}

void PlacemarkEditHeader::setLongitude(qreal longitude)
{
    d->setLongitude(longitude);
}

void PlacemarkEditHeader::setNotation(GeoDataCoordinates::Notation notation)
{
    d->setNotation(notation);
}

void PlacemarkEditHeader::setPositionVisible(bool visible)
{
    d->setPositionVisible(visible);
}

QString PlacemarkEditHeader::setId( const QString &id)
{
    return d->setId( id );
}

void PlacemarkEditHeader::setIdFilter( const QStringList &filter )
{
    d->setIdFilter( filter );
}

void PlacemarkEditHeader::setIdVisible(bool visible)
{
    d->setIdVisible( visible );
}

void PlacemarkEditHeader::setTargetId(const QString &targetId)
{
    d->setTargetId( targetId );
}

void PlacemarkEditHeader::setTargetIdList(const QStringList &targetIdList)
{
    d->setTargetIdList( targetIdList );
}

void PlacemarkEditHeader::setTargetIdVisible(bool visible)
{
    d->setTargetIdVisible( visible );
}

void PlacemarkEditHeader::selectAll()
{
    d->selectAll();
}

void PlacemarkEditHeader::setReadOnly(bool state)
{
    d->m_longitude->setDisabled(state);
    d->m_latitude->setDisabled(state);
    d->m_name->setReadOnly(state);
    d->m_iconLink->setReadOnly(state);
    d->iconLinkButton->setDisabled(state);
    d->m_id->setReadOnly(state);
    d->m_targetId->setDisabled(state);
    d->m_iconLink->setReadOnly(state);

    d->idLabel->setVisible(!state);
    d->m_id->setVisible(!state);
}

void PlacemarkEditHeader::updateValues()
{
    disconnect(d->m_id,  SIGNAL(textChanged(QString)),    this, SLOT(updateValues()));
    d->updateValues();
    connect(d->m_id,  SIGNAL(textChanged(QString)),    this, SLOT(updateValues()));
    emit valueChanged();
}



#include "moc_PlacemarkEditHeader.cpp"
