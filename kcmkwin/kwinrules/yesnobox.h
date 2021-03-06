/*
 * Copyright (c) 2011 Thomas Lübking <thomas.luebking@web.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#ifndef YESNOBOX_H
#define YESNOBOX_H

#include <QHBoxLayout>
#include <QRadioButton>

#include <KLocalizedString>

class YesNoBox : public QWidget {
    Q_OBJECT
public:
    explicit YesNoBox( QWidget *parent );
    bool isChecked() { return yes->isChecked(); }
public Q_SLOTS:
    void setChecked(bool b) { yes->setChecked(b); }
    void toggle() { yes->toggle(); }

Q_SIGNALS:
    void clicked(bool checked = false);
    void toggled(bool checked);
private Q_SLOTS:
    void noClicked(bool checked) { emit clicked(!checked); }
private:
    QRadioButton *yes, *no;
};

#endif // YESNOBOX_H
