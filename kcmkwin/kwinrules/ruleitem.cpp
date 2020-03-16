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

#include "ruleitem.h"


namespace KWin
{

class RuleItemPrivate
{
public:
    RuleItemPrivate(const QString &key, const QString &name, const QString &section, const QString &iconName)
    : m_key(key)
    , m_name(name)
    , m_section(section)
    , m_icon(QIcon::fromTheme(iconName))
    , m_enabled(false)
    , m_flags(0)
    {};

public:
    QString m_key;
    QString m_name;
    QString m_section;
    QIcon m_icon;

    bool m_enabled;

    QString m_description;
    uint m_flags;

    RuleItem::Type m_type;
    QVariant m_value;
};


RuleItem::RuleItem(const QString &key,
                   const RulePolicy::Type policyType,
                   const RuleItem::Type type,
                   const QString &name,
                   const QString &section,
                   const QString &iconName,
                   const QList<OptionsModel::Data> &options
                  )
    : d(new RuleItemPrivate(key, name, section, iconName))
    , p(new RulePolicy(policyType))
    , o(nullptr)
{
    d->m_type = type;
    if (type == Option || type == FlagsOption) {
        o = new OptionsModel(options);
    }

    reset();
}

RuleItem::~RuleItem()
{
    delete d;
    delete p;
    delete o;
}

void RuleItem::reset()
{
    d->m_enabled = hasFlag(AlwaysEnabled) | hasFlag(StartEnabled);
    d->m_value = typedValue(QVariant(), d->m_type);
    p->resetValue();
    if (o) {
        o->resetValue();
    }
}

QString RuleItem::key() const
{
    return d->m_key;
}

QString RuleItem::name() const
{
    return d->m_name;
}

QString RuleItem::section() const
{
    return d->m_section;
}

QString RuleItem::iconName() const
{
    return d->m_icon.name();
}

QIcon RuleItem::icon() const
{
    return d->m_icon;
}

QString RuleItem::description() const
{
    return d->m_description;
}

void RuleItem::setDescription(const QString& description)
{
    d->m_description = description;
}

bool RuleItem::isEnabled() const
{
    return d->m_enabled;
}

void RuleItem::setEnabled(bool enabled)
{
    d->m_enabled = enabled | hasFlag(AlwaysEnabled);
}

bool RuleItem::hasFlag(uint flag) const
{
    return (d->m_flags & flag) == flag;
}

void RuleItem::setFlags(uint flags, bool active)
{
    if (active) {
        d->m_flags |= flags;
    } else {
        d->m_flags &= (AllFlags ^ flags);
    }
}

RuleItem::Type RuleItem::type() const
{
    return d->m_type;
}

QVariant RuleItem::value() const
{
    if (d->m_type == Option) {
        return o->value();
    }
    return d->m_value;
}

void RuleItem::setValue(QVariant value)
{
    if (d->m_type == Option) {
        o->setValue(value);
    }
    d->m_value = typedValue(value, d->m_type);
}

QVariant RuleItem::options() const
{
    if (!o) {
        return QVariant();
    }
    return QVariant::fromValue(o);
}

void RuleItem::setOptionsData(const QList<OptionsModel::Data> &data)
{
    if (!o) {
        if (d->m_type != Option && d->m_type != FlagsOption) {
            return;
        }
        o = new OptionsModel();
    }
    o->updateModelData(data);
    o->setValue(d->m_value);
}

int RuleItem::policy() const
{
    return p->value();
}

void RuleItem::setPolicy(int policy)
{
    p->setValue(policy);
}

RulePolicy::Type RuleItem::policyType() const
{
    return p->type();
}

QVariant RuleItem::policyModel() const
{
    return QVariant::fromValue(p);
}

QString RuleItem::policyKey() const
{
    return p->policyKey(d->m_key);
}

QVariant RuleItem::typedValue(const QVariant &value, const RuleItem::Type type)
{
    switch (type) {
        case Undefined:
            return value;
        case Boolean:
            return value.toBool();
        case String:
            return value.toString();
        case Integer:
            return value.toInt();
        case Option:
            return value;
        case FlagsOption:
            return value.toUInt();
        case Percentage:
            return value.toUInt();
        case Coordinate:
            if (value.toString().isEmpty()) {
                return QStringLiteral("0,0");
            }
            return value.toString();
        case Shortcut:
            return value.toString();
    }

    return value;
}

}   //namespace

