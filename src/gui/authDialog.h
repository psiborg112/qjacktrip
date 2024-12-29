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

#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "../Auth.h"

namespace Ui
{
class AuthDialog;
}

class AuthDialog : public QDialog
{
    Q_OBJECT

   public:
    explicit AuthDialog(QWidget* parent = nullptr, const QString& credsFile = "", const QString& lastPath = "");
    ~AuthDialog() override;
    
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    
   signals:
    void signalFileChanged(const QString& fileName);

   private slots:
    void createFile();
    void browseForFile();
    void savePosition();

   private:
    QScopedPointer<Ui::AuthDialog> m_ui;
    QScopedPointer<Auth> m_auth;
    QString m_lastPath;
    
    bool m_changed = false;
};

#endif  // AUTHDIALOG_H
