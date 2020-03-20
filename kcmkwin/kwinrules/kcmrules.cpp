/*
 * Copyright (c) 2004 Lubos Lunak <l.lunak@kde.org>
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

#include "kcmrules.h"

#include <QFileDialog>
#include <QtDBus>

#include <KAboutData>
#include <KConfig>
#include <KLocalizedString>
#include <KPluginFactory>


namespace
{
    const QString s_configFile { QLatin1String("kwinrulesrc") };
}

namespace KWin
{

KCMKWinRules::KCMKWinRules(QObject *parent, const QVariantList &arguments)
    : KQuickAddons::ConfigModule(parent, arguments)
    , m_ruleBook(new RuleBookSettings(this))
    , m_rulesModel(new RulesModel(this))
{
    auto about = new KAboutData(QStringLiteral("kcm_kwinrules_qml"),
                                i18n("Window Rules"),
                                QStringLiteral("1.0"),
                                QString(),
                                KAboutLicense::GPL);
    about->addAuthor(i18n("Ismael Asensio"),
                     i18n("Author"),
                     QStringLiteral("isma.af@gmail.com"));
    setAboutData(about);

    setQuickHelp(i18n("<p><h1>Window-specific Settings</h1> Here you can customize window settings specifically only"
                      " for some windows.</p>"
                      " <p>Please note that this configuration will not take effect if you do not use"
                      " KWin as your window manager. If you do use a different window manager, please refer to its documentation"
                      " for how to customize window behavior.</p>"));

    connect(m_rulesModel, &RulesModel::descriptionChanged, this, [this]{
        if (m_editingIndex >=0 && m_editingIndex < m_ruleBook->count()) {
            m_rules.at(m_editingIndex)->description = m_rulesModel->description();
            emit ruleBookModelChanged();
        }
    } );
    connect(m_rulesModel, &RulesModel::dataChanged, this, &KCMKWinRules::updateNeedsSave);
}

KCMKWinRules::~KCMKWinRules() {
    delete m_ruleBook;
}


QStringList KCMKWinRules::ruleBookModel() const
{
    QStringList ruleDescriptionList;
    for (const Rules *rule : qAsConst(m_rules)) {
        ruleDescriptionList.append(rule->description);
    }
    return ruleDescriptionList;
}


void KCMKWinRules::load()
{
    m_ruleBook->load();
    m_rules = m_ruleBook->rules();

    setNeedsSave(false);
    emit ruleBookModelChanged();

    // Check if current index is no longer valid
    if (m_editingIndex >= m_rules.count()) {
        m_editingIndex = -1;
        pop();
        emit editingIndexChanged();
    }
    // Reset current index for rule editor
    if (m_editingIndex > 0) {
        m_rulesModel->importFromRules(m_rules.at(m_editingIndex));
    }
}

void KCMKWinRules::save()
{
    saveCurrentRule();
    m_ruleBook->setRules(m_rules);
    m_ruleBook->save();

    QDBusMessage message = QDBusMessage::createSignal("/KWin", "org.kde.KWin", "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}

void KCMKWinRules::updateState()
{
    m_ruleBook->setCount(m_rules.count());

    emit editingIndexChanged();
    emit ruleBookModelChanged();

    updateNeedsSave();
}

void KCMKWinRules::updateNeedsSave()
{
    setNeedsSave(true);
    emit needsSaveChanged();
}

void KCMKWinRules::saveCurrentRule()
{
    if (m_editingIndex < 0) {
        return;
    }
    if (needsSave()) {
        delete(m_rules[m_editingIndex]);
        m_rules[m_editingIndex] = m_rulesModel->exportToRules();
    }
}


int KCMKWinRules::editingIndex() const
{
    return m_editingIndex;
}

void KCMKWinRules::editRule(int index)
{
    if (index < 0 || index >= m_rules.count()) {
        return;
    }
    saveCurrentRule();

    m_editingIndex = index;
    m_rulesModel->importFromRules(m_rules.at(m_editingIndex));

    emit editingIndexChanged();

    // Show and move to Rules Editor page
    if (depth() < 2) {
        push(QStringLiteral("RulesEditor.qml"));
    }
    setCurrentIndex(1);
}

void KCMKWinRules::newRule()
{

    m_rules.append(new Rules());
    updateState();

    const int newIndex = m_rules.count() - 1;
    editRule(newIndex);

    saveCurrentRule();
}

void KCMKWinRules::removeRule(int index)
{
    const int lastIndex = m_rules.count() - 1;
    if (index < 0 || index > lastIndex) {
        return;
    }

    if (m_editingIndex == index) {
        m_editingIndex = -1;
        pop();
    }

    m_rules.removeAt(index);

    updateState();
}

void KCMKWinRules::moveRule(int sourceIndex, int destIndex)
{
    const int lastIndex = m_rules.count() - 1;
    if (sourceIndex == destIndex
            || (sourceIndex < 0 || sourceIndex > lastIndex)
            || (destIndex < 0 || destIndex > lastIndex)) {
        return;
    }

    m_rules.move(sourceIndex, destIndex);

    if (m_editingIndex == sourceIndex) {
        m_editingIndex = destIndex;
        emit editingIndexChanged();
    } else if (m_editingIndex > sourceIndex && m_editingIndex <= destIndex) {
        m_editingIndex -= 1;
        emit editingIndexChanged();
    } else if (m_editingIndex < sourceIndex && m_editingIndex >= destIndex) {
        m_editingIndex += 1;
        emit editingIndexChanged();
    }

    emit ruleBookModelChanged();
}

void KCMKWinRules::exportRule(int index)
{
    Q_ASSERT(index >= 0 && index < m_rules.count());

    saveCurrentRule();

    const QString description = m_rules.at(index)->description;
    const QString defaultPath = QDir(QDir::home()).filePath(description + QStringLiteral(".kwinrule"));
    const QString path = QFileDialog::getSaveFileName(nullptr, i18n("Export Rules"), defaultPath,
                                                      i18n("KWin Rules (*.kwinrule)"));
    if (path.isEmpty())
        return;

    const auto config = KSharedConfig::openConfig(path, KConfig::SimpleConfig);
    RuleSettings settings(config, m_rules.at(index)->description);

    settings.setDefaults();
    m_rules.at(index)->write(&settings);
    settings.save();
}

void KCMKWinRules::importRules()
{
    QString path = QFileDialog::getOpenFileName(nullptr,
                                                i18n("Import Rules"),
                                                QDir::home().absolutePath(),
                                                i18n("KWin Rules (*.kwinrule)"));

    if (path.isEmpty()) {
        return;
    }

    const auto config = KSharedConfig::openConfig(path, KConfig::SimpleConfig);
    const QStringList groups = config->groupList();
    if (groups.isEmpty()) {
        return;
    }

    for (const QString &groupName : groups) {
        RuleSettings settings(config, groupName);

        const bool remove = settings.deleteRule();
        const QString importDescription = settings.description();
        if (importDescription.isEmpty()) {
            continue;
        }

        // Try to find a rule with the same description to replace
        int newIndex = -2;
        for (int index = 0; index < m_rules.count(); index++) {
            if (m_rules.at(index)->description == importDescription) {
                newIndex = index;
                break;
            }
        }

        if (remove) {
            removeRule(newIndex);
            continue;
        }

        Rules *newRule = new Rules(&settings);

        if (newIndex < 0) {
            m_rules.append(newRule);
        } else {
            delete m_rules[newIndex];
            m_rules[newIndex] = newRule;
        }

        // Reset rule editor if the current rule changed when importing
        if (m_editingIndex == newIndex) {
            m_rulesModel->importFromRules(m_rules.at(m_editingIndex));
        }
    }

    updateState();
}

K_PLUGIN_CLASS_WITH_JSON(KCMKWinRules, "kcm_kwinrules_qml.json");

} // namespace

#include "kcmrules.moc"
