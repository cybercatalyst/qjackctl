/****************************************************************************
   Copyright (C) 2015, Jacob Dawid <jacob@omg-it.works>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#pragma once

// Qt includes
#include <QApplication>
#include <QObject>

// Own includes
#include "settings.h"

class JackControl :
    public QObject {
    Q_OBJECT
public:
    static JackControl& instance() {
        static JackControl jackControl;
        return jackControl;
    }

    ~JackControl() {
    }

    void initialize(int& argc, char **argv);
    int run();

    Settings::JackServerPreset currentPreset();

public slots:
    void setCurrentPreset(Settings::JackServerPreset jackServerPreset);

signals:
    void currentPresetChanged(Settings::JackServerPreset preset);

private:
    JackControl() :
        QObject() {
    }

    QApplication *_application;

    Settings::JackServerPreset _currentPreset;
};
