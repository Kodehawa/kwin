/*
 * Copyright (c) 2020 Ismael Asensio <isma.af@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <rules.h>
#include <rulesmodel.h>

#include <QStringListModel>

#include <KConfig>
#include <KQuickAddons/ConfigModule>


namespace KWin
{

class KCMKWinRules : public KQuickAddons::ConfigModule
{
    Q_OBJECT

    Q_PROPERTY(QStringList rulesListModel READ rulesListModel NOTIFY rulesListModelChanged)
    Q_PROPERTY(int editingIndex READ editingIndex() NOTIFY editingIndexChanged)
    Q_PROPERTY(RulesModel *rulesModel MEMBER m_rulesModel CONSTANT)

public:
    explicit KCMKWinRules(QObject *parent, const QVariantList &arguments);
    ~KCMKWinRules();

public slots:
    void load() override;
    void save() override;

    int editingIndex() const;
    void editRule(int index);

    void newRule();
    void removeRule(int index);
    void moveRule(int sourceIndex, int destIndex);

    void exportRule(int index);
    void importRules();

signals:
    void rulesListModelChanged();
    void editingIndexChanged();

private slots:
    void updateNeedsSave();
    void updateState();

private:
    QStringList rulesListModel() const;
    KConfigGroup rulesConfigGroup(int index) const;

    void saveCurrentRule();
    void moveConfigGroup(int sourceIndex, int destIndex);
    void pushRulesEditor();

private:
    KConfig *m_rulesConfig;
    QStringList m_rulesListModel;
    RulesModel* m_rulesModel;

    int m_editingIndex = -1;
};

} // namespace
