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


QQC2.ComboBox {
    id: optionsCombo

    textRole: "text"
    //FIXME: After Qt 5.14 this can be replaced by the new implemented properties:
    //   valueRole: "value"`
    //   currentValue: model.value

    property var currentValue
    property var values: []

    currentIndex: model.selectedIndex

    onActivated: (index) => {
        currentValue = values[index];
    }

    delegate: QQC2.ItemDelegate {
        readonly property bool reversed : Qt.application.layoutDirection === Qt.RightToLeft

        highlighted: optionsCombo.highlightedIndex == index
        width: parent.width

        LayoutMirroring.enabled: reversed
        LayoutMirroring.childrenInherit: true

        contentItem: RowLayout {
            Kirigami.Icon {
                source: model.icon
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
            }
            QQC2.Label {
                text: model.text
                color: highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft
            }
        }

        QQC2.ToolTip {
            text: model.description
            visible: hovered && (model.description != "")
        }

        Component.onCompleted: {
            values[index] = model.value;
            optionsCombo.popup.width = Math.max(implicitWidth, optionsCombo.width, optionsCombo.popup.width);
        }
    }
}
