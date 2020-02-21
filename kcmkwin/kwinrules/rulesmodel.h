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

#ifndef KWIN_RULES_MODEL_H
#define KWIN_RULES_MODEL_H

#include "ruleitem.h"
#include <rules.h>

#include <QAbstractListModel>
#include <QObject>

#ifdef KWIN_BUILD_ACTIVITIES
#include <KActivities/Consumer>
#endif

#include <KConfig>


namespace KWin
{

class RulesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool showWarning READ isWarningShown NOTIFY showWarningChanged)

public:
    enum RulesRole {
        NameRole = Qt::DisplayRole,
        DescriptionRole = Qt::ToolTipRole,
        IconRole = Qt::DecorationRole,
        IconNameRole = Qt::UserRole + 1,
        KeyRole,
        SectionRole,
        EnabledRole,
        SelectableRole,
        ValueRole,
        TypeRole,
        PolicyRole,
        PolicyModelRole,
        OptionsModelRole
    };

public:
    explicit RulesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;

    bool hasRule(const QString &key) const;
    RuleItem *ruleItem(const QString &key) const;

    void readFromConfig(KConfigGroup *config);
    void writeToConfig(KConfigGroup *config) const;
    void prefillProperties(const QVariantMap &info);

    void importFromRules(Rules *rules);
    Rules *exportToRules() const;

    bool isWarningShown();

public slots:
    void init();

signals:
    void showWarningChanged();

private:
    void initRuleList();
    void initPropertyMap();
    RuleItem *addRule(RuleItem *rule);
    const QString defaultDescription() const;

    QList<OptionsModel::Data> windowTypesModelData() const;
    QList<OptionsModel::Data> virtualDesktopsModelData() const;
    QList<OptionsModel::Data> activitiesModelData() const;
    QList<OptionsModel::Data> placementModelData() const;
    QList<OptionsModel::Data> focusModelData() const;
    QList<OptionsModel::Data> colorSchemesModelData() const;

private:
    QList<RuleItem *> m_ruleList;
    QHash<QString, RuleItem *> m_rules;

    QHash<QString, QString> m_ruleForProperty;
#ifdef KWIN_BUILD_ACTIVITIES
    KActivities::Consumer *m_activities;
#endif
};

}

#endif
