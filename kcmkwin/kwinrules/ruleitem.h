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

#ifndef KWIN_RULEITEM_H
#define KWIN_RULEITEM_H

#include "ruleitem_p.h"

#include <QIcon>


namespace KWin
{

class RuleItem : public QObject
{
    Q_OBJECT

public:
    enum Flags {
        NoFlags            = 0,
        AlwaysEnabled      = 1u << 0,
        StartEnabled       = 1u << 1,
        AffectsWarning     = 1u << 2,
        AffectsDescription = 1u << 3,
        AllFlags           = 0b1111
    };

public:
    RuleItem(const QString &key,
             const RulePolicy::Type policyType,
             const RuleType type,
             const QString &name,
             const QString &section,
             const QString &iconName = QStringLiteral("window"),
             const QList<OptionsModel::Data> &options = {}
            );
    ~RuleItem();

    QString key() const;
    QString name() const;
    QString section() const;
    QString iconName() const;
    QIcon icon() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    QString description() const;
    void setDescription(const QString &description);

    bool hasFlag(uint flag) const;
    void setFlags(uint flags, bool active=true);

    RuleType type() const;
    QVariant value() const;
    void setValue(QVariant value);

    QVariant options() const;

    RulePolicy::Type policyType() const;
    int policy() const;          // int belongs to anonymous enum in Rules::
    void setPolicy(int policy);  // int belongs to anonymous enum in Rules::
    QVariant policyModel() const;
    QString policyKey() const;

    void reset();

public slots:
    void setOptionsData(const QList<OptionsModel::Data> &data);

signals:
    void valueChanged(QVariant value);
    void policyChanged(int policy);

private:
    static QVariant typedValue(const QVariant &value, const RuleType type);

private:
    RuleItemPrivate *d;
    RulePolicy *p;
    OptionsModel *o;
};

}   //namespace

#endif  //KWIN_RULEITEM_H
