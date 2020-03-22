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
#include "rulebooksettings.h"
#include "rulesmodel.h"

#include <QStringListModel>

#include <KQuickAddons/ConfigModule>


namespace KWin
{

class KCMKWinRules : public KQuickAddons::ConfigModule
{
    Q_OBJECT

    Q_PROPERTY(QStringList ruleBookModel READ ruleBookModel NOTIFY ruleBookModelChanged)
    Q_PROPERTY(int editingIndex READ editingIndex NOTIFY editingIndexChanged)
    Q_PROPERTY(RulesModel *rulesModel MEMBER m_rulesModel CONSTANT)

public:
    explicit KCMKWinRules(QObject *parent, const QVariantList &arguments);
    ~KCMKWinRules() override;

    Q_INVOKABLE void editRule(int index);

    Q_INVOKABLE void createRule();
    Q_INVOKABLE void removeRule(int index);
    Q_INVOKABLE void moveRule(int sourceIndex, int destIndex);

    Q_INVOKABLE void exportToFile(const QUrl &path, int index);
    Q_INVOKABLE void importFromFile(const QUrl &path);

public slots:
    void load() override;
    void save() override;

signals:
    void ruleBookModelChanged();
    void editingIndexChanged();

private slots:
    void updateNeedsSave();
    void updateState();

private:
    QStringList ruleBookModel() const;
    int editingIndex() const;
    void saveCurrentRule();

private:
    RuleBookSettings *m_ruleBook;
    QVector<Rules *> m_rules;
    RulesModel* m_rulesModel;

    int m_editingIndex = -1;
};

} // namespace
