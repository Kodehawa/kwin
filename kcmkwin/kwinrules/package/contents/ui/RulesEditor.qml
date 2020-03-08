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

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as QQC2
import org.kde.kirigami 2.10 as Kirigami
import org.kde.kcm 1.2


ScrollViewKCM {
    id: rulesEditor
    property var rulesModel: kcm.rulesModel

    title: rulesModel.description

    header: RowLayout {
        id: filterBar
        Kirigami.ActionTextField {
            id: searchField

            Layout.fillWidth: true
            placeholderText: i18n("Search...")
            focusSequence: "Ctrl+F"

            rightActions: [
                Kirigami.Action {
                    iconName: "edit-clear"
                    visible: searchField.text !== ""
                    onTriggered: {
                        searchField.text = ""
                        searchField.accepted()
                    }
                }
            ]
        }
        QQC2.ToolButton {
            id: showAllButton
            icon.name: checked ? 'view-visible' : 'view-hidden'
            checkable: true
            text: i18n("Show all rules")
        }
    }

    view: ListView {
        id: enabledRulesView
        clip: true

        model: rulesModel
        delegate: RuleItemDelegate {
            showAll: showAllButton.checked
        }
        section {
            delegate: Kirigami.ListSectionHeader { label: section }
            property: "section"
        }
    }

    // FIXME: InlineMessage.qml:241:13: QML Label: Binding loop detected for property "verticalAlignment"
    footer: Kirigami.InlineMessage {
        id: warningMessage
        visible: rulesModel.showWarning
        text: i18n("You have specified the window class as unimportant.\n" +
                   "This means the settings will possibly apply to windows from all " +
                   "applications. If you really want to create a generic setting, it is " +
                   "recommended you at least limit the window types to avoid special window " +
                   "types.")
    }
}
