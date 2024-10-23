// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "ExternalEditorDialog.h"

#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>

namespace Marble
{

namespace
{
QString const merkaartor = QStringLiteral("merkaartor");
QString const josm = QStringLiteral("josm");
QString const potlatch = QStringLiteral("potlatch");
}

class ExternalEditorDialogPrivate
{
public:
    QString m_defaultEditor;

    QMap<QString, bool> m_installedEditors;

    ExternalEditorDialogPrivate();
};

ExternalEditorDialogPrivate::ExternalEditorDialogPrivate()
    : m_defaultEditor(potlatch)
{
    QString path = QProcessEnvironment::systemEnvironment().value(QStringLiteral("PATH"), QStringLiteral("/usr/local/bin:/usr/bin:/bin"));
    auto const applications = QStringList() << merkaartor << josm;
    for (const QString &application : applications) {
        m_installedEditors[application] = false;
        /** @todo: what's the qt way to get the path entry separator? Will be a semicolon on Windows */
        for (const QString &dir : path.split(QLatin1Char(':'))) {
            QFileInfo executable(QDir(dir), application);
            if (executable.exists()) {
                m_installedEditors[application] = true;
                break;
            }
        }
    }
}

ExternalEditorDialog::ExternalEditorDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , d(new ExternalEditorDialogPrivate)
{
    setupUi(this);

    connect(editorComboBox, &QComboBox::currentIndexChanged, this, &ExternalEditorDialog::updateDefaultEditor);

    if (d->m_installedEditors[merkaartor]) {
        d->m_defaultEditor = merkaartor;
        editorComboBox->setCurrentIndex(1);
    } else if (d->m_installedEditors[josm]) {
        d->m_defaultEditor = josm;
        editorComboBox->setCurrentIndex(2);
    }
}

ExternalEditorDialog::~ExternalEditorDialog()
{
    delete d;
}

QString ExternalEditorDialog::externalEditor() const
{
    return d->m_defaultEditor;
}

bool ExternalEditorDialog::saveDefault() const
{
    return saveDefaultCheckBox->isChecked();
}

void ExternalEditorDialog::updateDefaultEditor(int index)
{
    QString description;

    switch (index) {
    case 1:
        d->m_defaultEditor = merkaartor;
        description = tr("Merkaartor is an OpenStreetMap editor that is powerful and easy to use. It integrates well into the used workspace.");
        if (!d->m_installedEditors[d->m_defaultEditor]) {
            description += QLatin1StringView(" <b>")
                + tr("Please ask your system administrator to install %1 on your system.").arg(QStringLiteral("Merkaartor")) + QLatin1StringView("</b>");
        }
        break;
    case 2:
        d->m_defaultEditor = josm;
        description =
            tr("JOSM is a powerful OpenStreetMap editor which is more complex to use than other editors. It is built on the Java platform and therefor runs on "
               "all systems for which Java is available but does not integrate well into the workspace. A Java SE-compatible runtime is required.");
        if (!d->m_installedEditors[d->m_defaultEditor]) {
            description += QLatin1StringView(" <b>") + tr("Please ask your system administrator to install %1 on your system.").arg(QStringLiteral("JOSM"))
                + QLatin1StringView("</b>");
        }
        break;
    default:
        d->m_defaultEditor = potlatch;
        description =
            tr("iD is a very easy to use OpenStreetMap editor, though lacks the power of Merkaartor and JOSM. It runs on all platforms with a web browser.");
        break;
    }

    screenshotLabel->setPixmap(QPixmap(QLatin1StringView(":/data/editors/") + d->m_defaultEditor + QLatin1StringView(".png")));
    descriptionLabel->setText(description);
}

}

#include "moc_ExternalEditorDialog.cpp"
