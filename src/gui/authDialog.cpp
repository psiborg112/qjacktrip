//*****************************************************************
/*
  QJackTrip: Bringing a graphical user interface to JackTrip, a
  system for high quality audio network performance over the
  internet.

  Copyright (c) 2022 Aaron Wyatt.

  This file is part of QJackTrip.

  QJackTrip is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  QJackTrip is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with QJackTrip.  If not, see <https://www.gnu.org/licenses/>.
*/
//*****************************************************************

#include "authDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "ui_authDialog.h"

AuthDialog::AuthDialog(QWidget* parent, const QString& credsFile, const QString& lastPath) 
    : QDialog(parent)
    , m_ui(new Ui::AuthDialog)
    , m_lastPath(lastPath)
{
    m_ui->setupUi(this);
    m_ui->credsEdit->setText(credsFile);
    m_auth.reset(new Auth());
    
    connect(m_ui->browseButton, &QPushButton::clicked, this, &AuthDialog::browseForFile);
    connect(m_ui->newButton, &QPushButton::clicked, this, &AuthDialog::createFile);
}

void AuthDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    QSettings settings;
    settings.beginGroup(QStringLiteral("Window"));
    QByteArray geometry = settings.value("AuthGeometry").toByteArray();
    if (geometry.size() > 0) {
        restoreGeometry(geometry);
    }
    if (m_ui->credsEdit->text().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("The location of the file containing usernames and passwords hasn't been set yet. Do you want to create a new one?");
        msgBox.setWindowTitle(QStringLiteral("Create Users File?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (msgBox.exec() == QMessageBox::Yes) {
            createFile();
        }
    } else {
        
    }
}

void AuthDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);
    savePosition();
}

void AuthDialog::createFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Create File", m_lastPath + QStringLiteral("/jacktrip-users"), "");
    if (!fileName.isEmpty()) {
        m_ui->credsEdit->setText(fileName);
        m_ui->credsEdit->setFocus(Qt::OtherFocusReason);
        m_lastPath = QFileInfo(fileName).canonicalPath();
        m_auth->setFileName(m_ui->credsEdit->text(), false);

        m_ui->addUserButton->setEnabled(true);
        m_ui->usersTableView->setEnabled(true);
        m_ui->saveButton->setEnabled(true);
        m_changed = true;
    }
}

void AuthDialog::browseForFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", m_lastPath, "");
    if (!fileName.isEmpty()) {
        m_ui->credsEdit->setText(fileName);
        m_ui->credsEdit->setFocus(Qt::OtherFocusReason);
        m_lastPath = QFileInfo(fileName).canonicalPath();
        m_auth->setFileName(m_ui->credsEdit->text(), true);
        
        m_ui->addUserButton->setEnabled(true);
        m_ui->usersTableView->setEnabled(true);
    }
}

void AuthDialog::savePosition()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("Window"));
    settings.setValue("AuthGeometry", saveGeometry());
    settings.endGroup();
}

AuthDialog::~AuthDialog() = default;
