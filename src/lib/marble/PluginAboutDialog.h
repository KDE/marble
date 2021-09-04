// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_PLUGINABOUTDIALOG_H
#define MARBLE_PLUGINABOUTDIALOG_H

#include <QDialog>
#include <QList>

// Marble
#include "marble_export.h"
#include "PluginInterface.h"

namespace Marble
{

class PluginAboutDialogPrivate;

class MARBLE_EXPORT PluginAboutDialog : public QDialog
{
    Q_OBJECT

 public:
    enum LicenseKey {
//        License_Unknown = 0,
//        License_GPL = 1,
//        License_GPL_V2 = 1,
//        License_LGPL = 2,
        License_LGPL_V2 = 2
//        License_BSD = 3,
//        License_Artistic = 4,
//        License_QPL = 5,
//        License_QPL_V1_0 = 5,
//        License_GPL_V3 = 6,
//        License_LGPL_V3 = 7
    };

    explicit PluginAboutDialog( QWidget *parent = nullptr );
    ~PluginAboutDialog() override;

    /**
     * Sets the name of the plugin.
     */
    void setName( const QString& name );

    /**
     * Sets the version of the plugin;
     */
    void setVersion( const QString& version );

    /**
     * Sets the icon to be displayed at the top of the dialog.
     */
    void setIcon( const QIcon& icon );

    /**
     * Sets the text displayed in the "About" tab of the dialog.
     */
    void setAboutText( const QString& about );

    /**
     * Sets the authors working on this plugin.
     * @since 0.26.0
     */
    void setAuthors(const QVector<PluginAuthor>& authors);

    /**
     * Sets the text displayed in the "Authors" tab of the dialog.
     * @warning You would want to use setAuthors instead.
     */
    void setAuthorsText( const QString& authors );

    /**
     * Sets the text displayed in the "Data" tab of the dialog.
     * An null QString will result into no "Data" tab at all.
     */
    void setDataText( const QString& data );

    /**
     * Sets the license for the "License Agreement" tab of the dialog.
     */
    void setLicense( PluginAboutDialog::LicenseKey license );

    /**
     * Sets the text displayed in the "License Agreement" tab of the dialog.
     * A null QString will result into the default LGPLv2 license agreement.
     * If your plugin is not part of the official Marble distribution, please set it
     * manually to the license agreement you prefer (even if it is LGPLv2).
     * @warning You would want to use setLicense instead.
     */
    void setLicenseAgreementText( const QString& license );

 private:
    Q_DISABLE_COPY( PluginAboutDialog )

    PluginAboutDialogPrivate * const d;
};

} // namespace Marble

#endif
