//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
//

#ifndef EDITPLACEMARKDIALOG_H
#define EDITPLACEMARKDIALOG_H

#include "marble_export.h"

#include <QDialog>

namespace Marble {

class GeoDataFeature;
class GeoDataPlacemark;
class OsmPlacemarkData;

/**
 * @brief The EditPlacemarkDialog class deals with customizing placemarks.
 */
class MARBLE_EXPORT EditPlacemarkDialog : public QDialog
{
    Q_OBJECT

public:
    EditPlacemarkDialog( GeoDataPlacemark *placemark,
                         const QHash<qint64,OsmPlacemarkData> *relations = 0,
                         QWidget *parent = 0 );
    ~EditPlacemarkDialog();

    /**
     * @brief setLabelColor tells the dialog what the label color is
     */
    void setLabelColor( const QColor &color );

    /**
     * @brief idFilter gets filter for id of placemark
     * @return QStringList of ids which could not be used as id.
     */
    QStringList idFilter() const;

    /**
     * @brief targetIds gets ids which could be target of placemark.
     * @return QStringList of ids which could be target of placemark.
     */
    QStringList targetIds() const;

    /**
     * @brief isTargetIdFieldVisible tells if targetId field is shown.
     */
    bool isTargetIdFieldVisible() const;

    /**
     * @brief isIdFieldVisible tells if targetId field is shown.
     */
    bool isIdFieldVisible() const;

public Q_SLOTS:
    /**
     * @brief updateDialogFields is connected to a signal from AnnotatePlugin in order
     * to update some fields in the dialog as the user interacts directly with the text
     * annotation item.
     */
    void updateDialogFields();

    /**
     * @brief setIdFilter sets filter for id of placemark.
     * @param filter QStringList with ids which could not be used as id.
     */
    void setIdFilter( const QStringList &filter );

    /**
     * @brief setTargetIds sets ids which could be target of placemark.
     * @param targetIds QStringList with ids which could be target of placemark.
     */
    void setTargetIds( const QStringList &targetIds );

    /**
     * @brief setTargetIdFieldVisible tells the dialog whether targetId field should be shown.
     */
    void setTargetIdFieldVisible( bool visible );

    /**
     * @brief setIdFieldVisible tells the dialog whether id field should be shown.
     */
    void setIdFieldVisible( bool visible );

    /**
     * @brief Protecting data from input fields changes
     */
    void setReadOnly( bool state );

private Q_SLOTS:

    /**
     * @brief checkFields shows warnings if there are important fields which don't hold
     * accurate information.
     */
    void checkFields();

    /**
     * @brief updateTextAnnotation is the main slot which synchronizes the information
     * from the dialog with the way the text annotation item is painted.
     */
    void updateTextAnnotation();

    /**
     * @brief updateLabelDialog The color chooser for label is represented as a push
     * button with a filled pixmap as its icon. This slot updates the color fill of this
     * pixmap.
     */
    void updateLabelDialog( const QColor &color );

    /**
     * @brief updateIconDialog The same as above, but for icons.
     * FIXME: This is not functional ATM - we need some mechanism for customizing existing
     * icons.
     */
    void updateIconDialog( const QColor &color );

    /**
     * @brief updatePlacemarkAltitude changes an actual elevation value of placemark instance
     * according to the value/unit of elevation widget spin box representing it
     */
    void updatePlacemarkAltitude();

    /**
     * @brief restoreInitial restores the dialog's previous settings if the dialog closes with
     * a zero return value.
     */
    void restoreInitial( int result );

    /**
     * @brief toogleDescriptionEditMode toggles edit mode for description field.
     */

Q_SIGNALS:
    /**
     * @brief textAnnotationUpdated signals that some property of the PlacemarkTextAnnotation
     * instance has changed.
     * @param feature The instance's placemark.
     */
    void textAnnotationUpdated( GeoDataFeature *feature );

    /**
     * @brief relationCreated signals the annotate plugin that a new relation has been
     * created( or modified ) within the relation editor
     * @param relation the relation's osmData
     */
    void relationCreated( const OsmPlacemarkData &relation );

private:
    class Private;
    Private * const d;
};

}

#endif
