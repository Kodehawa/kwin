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

    property bool multipleChoice: false
    property int selectionMask: 0
    property var itemText: []

    currentIndex: model.selectedIndex

    onActivated: (index) => {
        currentValue = values[index];
    }

    onSelectionMaskChanged: {
        if (multipleChoice) {
            displayText = selectionText();
        }
    }

    onCountChanged: {
        selectionMaskChanged();
    }

    delegate: QQC2.ItemDelegate {
        readonly property bool reversed : Qt.application.layoutDirection === Qt.RightToLeft

        highlighted: optionsCombo.highlightedIndex == index
        width: parent.width

        LayoutMirroring.enabled: reversed
        LayoutMirroring.childrenInherit: true

        contentItem: RowLayout {
            QQC2.CheckBox {
                id: itemSelection
                visible: multipleChoice
                checked: (selectionMask & (1 << value))
                onToggled: {
                    selectionMask = (selectionMask & ~(1 << value)) | (checked << value);
                    activated(index);
                }
            }
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
            itemText[index] = model.text;
            optionsCombo.popup.width = Math.max(implicitWidth, optionsCombo.width, optionsCombo.popup.width);
        }

        onFocusChanged: {
            if (!focus) popup.close();
        }
    }

    function selectionText() {
        var selectionCount = selectionMask.toString(2).replace(/0/g, '').length;
        switch (selectionCount) {
            case 0: return i18n("None selected");
            case count: return i18n("All selected");
            case 1: break;
            default: return i18np("1 selected", "%1 selected", selectionCount);
        }
        // Use the display text of the only selected item
        for (var i = 0; i < count; i++) {
            if (selectionMask & (1 << values[i])) return itemText[i];
        }
    }
}
