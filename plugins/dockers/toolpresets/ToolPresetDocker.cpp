/*
 *  Copyright (c) 2020 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ToolPresetDocker.h"

#include <QApplication>
#include <QAction>
#include <QThread>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QStandardPaths>

#include <KoDialog.h>

#include <klocalizedstring.h>
#include <kactioncollection.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include <KoToolRegistry.h>
#include <kis_image_config.h>
#include <kis_icon.h>
#include <KoCanvasBase.h>
#include <KisViewManager.h>
#include <kis_canvas2.h>
#include <KisMainWindow.h>
#include "kis_signal_compressor.h"

#include <KisPart.h>
#include <kis_types.h>
#include <KisViewManager.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoToolBase.h>
#include <KoToolFactoryBase.h>
#include <kis_canvas_resource_provider.h>
#include <KisDialogStateSaver.h>
#include <kis_paintop_preset.h>
#include <kis_paintop_settings.h>
#include <kis_categorized_list_view.h>
#include <kis_categorized_item_delegate.h>

#include "ToolPresetModel.h"

DlgNewPreset::DlgNewPreset()
    : KoDialog(KisPart::instance()->currentMainwindow())
{
    setButtons(KoDialog::Ok | KoDialog::Cancel);

    QWidget *page = new QWidget;
    m_ui.setupUi(page);
    setMainWidget(page);
}

QString DlgNewPreset::name()
{
    return m_ui.txtName->text();
}

bool DlgNewPreset::executeOnSelection()
{
    return m_ui.chkExecuteToolOnSelection->isChecked();
}

bool DlgNewPreset::saveResourcesWithPreset()
{
    return m_ui.chkSaveBrushPresetInformation->isChecked();
}

void DlgNewPreset::accept()
{
    if (m_ui.txtName->text().isEmpty()) {
        QMessageBox::warning(qApp->activeWindow(), i18nc("@title:window", "Krita"), i18n("Please enter a name for the tool preset."));
    }
    else {
        KoDialog::accept();
    }
}

ToolPresetDocker::ToolPresetDocker()
    : QDockWidget(i18n("Tool Option Presets"))
{
    setFeatures(DockWidgetMovable|DockWidgetFloatable);

    QWidget *page = new QWidget(this);
    setupUi(page);
    setWidget(page);

    bnAdd->setIcon(koIcon("document-new"));
    bnDelete->setIcon(koIcon("edit-delete"));

    m_toolPresetModel = new ToolPresetModel();
    lstPresets->setModel(m_toolPresetModel);
    lstPresets->setItemDelegate(new KisCategorizedItemDelegate(this));

    connect(KoToolManager::instance(), SIGNAL(toolOptionWidgetsChanged(KoCanvasController*,QList<QPointer<QWidget> >)), this, SLOT(optionWidgetsChanged(KoCanvasController*,QList<QPointer<QWidget> >)));
    connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController *, int)), this, SLOT(toolChanged(KoCanvasController *, int)));
    connect(bnAdd, SIGNAL(clicked()), SLOT(bnAddPressed()));
    connect(bnDelete, SIGNAL(clicked()), SLOT(bnDeletePressed()));
    connect(lstPresets, SIGNAL(clicked(QModelIndex)), SLOT(presetSelected(QModelIndex)));
}

ToolPresetDocker::~ToolPresetDocker()
{
    delete m_toolPresetModel;
}

void ToolPresetDocker::setViewManager(KisViewManager *kisview)
{
    m_resourceProvider = kisview->canvasResourceProvider();
}

void ToolPresetDocker::setCanvas(KoCanvasBase *canvas)
{
    m_canvas = canvas;
    setEnabled(canvas != 0);
}

void ToolPresetDocker::unsetCanvas()
{
    m_canvas = 0;
    setEnabled(false);
}

void ToolPresetDocker::optionWidgetsChanged(KoCanvasController */*canvasController*/, QList<QPointer<QWidget> > optionWidgets)
{
    m_currentOptionWidgets = optionWidgets;
}

void ToolPresetDocker::toolChanged(KoCanvasController *canvasController, int /*toolId*/)
{
    m_currentToolId = KoToolManager::instance()->activeToolId();
}

void ToolPresetDocker::bnAddPressed()
{
    DlgNewPreset dlg;
    QDialog::DialogCode result = (QDialog::DialogCode) dlg.exec();
    if (result == QDialog::Accepted) {

        QString section = dlg.name();
        QString optionFile = createConfigFileName(m_currentToolId);
        Q_FOREACH (QPointer<QWidget> widget, m_currentOptionWidgets) {
            if (widget) {
                KisDialogStateSaver::saveState(widget, section, optionFile);
            }
        }

        KoToolBase *tool = KoToolManager::instance()->toolById(m_canvas, m_currentToolId);
        if (tool && tool->inherits("KisToolPaint")) {
            KConfig cfg(createConfigFileName(m_currentToolId));
            KConfigGroup grp = cfg.group(dlg.name());
            KisPaintOpSettingsSP settings = m_resourceProvider->currentPreset()->settings();
            grp.writeEntry("execute_on_activate", dlg.executeOnSelection());
            grp.writeEntry("store_resources", dlg.saveResourcesWithPreset());
            if (dlg.saveResourcesWithPreset()) {
                KisPaintOpSettingsSP settings = m_resourceProvider->currentPreset()->settings();
                grp.writeEntry("brush_size", settings->paintOpSize());
                grp.writeEntry("paintop_preset", m_resourceProvider->currentPreset()->paintOp().id());
                grp.writeEntry("opacity", m_resourceProvider->opacity());
                grp.writeEntry("flow", m_resourceProvider->flow());
                grp.writeEntry("compositeop", m_resourceProvider->currentCompositeOp());
                grp.writeEntry("eraser", m_resourceProvider->eraserMode());
                grp.writeEntry("disablePressure", m_resourceProvider->disablePressure());
            }

        }

        m_toolPresetModel->addToolPreset(m_currentToolId, dlg.name());
    }

}

void ToolPresetDocker::bnDeletePressed()
{
    QModelIndex idx = lstPresets->currentIndex();
    if (idx.isValid()) {
        QString presetName = m_toolPresetModel->data(idx).toString();

        KConfig cfg(createConfigFileName(m_currentToolId), KConfig::SimpleConfig);
        cfg.deleteGroup(presetName);

        m_toolPresetModel->removeToolPreset(m_currentToolId, presetName);
    }
}

void ToolPresetDocker::presetSelected(QModelIndex idx)
{
    ToolPresetInfo *info = m_toolPresetModel->toolPresetInfo(idx.row());

    if (info->toolId != m_currentToolId) {
        KoToolManager::instance()->switchToolRequested(info->toolId);
    }

    //    QString text = info->presetName;
    //    txtName->setText(text);

    //    Q_FOREACH (QPointer<QWidget> widget, m_currentOptionWidgets) {
    //        if (widget) {
    //            KisDialogStateSaver::restoreState(widget, text, QMap<QString, QVariant>(), createConfigFileName(m_currentToolId));
    //        }
    //    }

    //    KConfig cfg(createConfigFileName(m_currentToolId));
    //    KConfigGroup grp = cfg.group(text);

    //    chkExecuteToolOnSelection->setChecked(grp.readEntry("execute_on_select", false));
    //    chkSaveBrushPresetInformation->setChecked(grp.readEntry("save_resources", false));

    //    KoToolBase *tool = KoToolManager::instance()->toolById(m_canvas, m_currentToolId);
    //    if (tool && tool->inherits("KisToolPaint")) {
    //        m_resourceProvider->setSize(grp.readEntry("brush_size", m_resourceProvider->size()));
    //    }

}
