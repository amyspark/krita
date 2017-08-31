/*
 *  Copyright (c) 2017 Boudewijn Rempt <boud@valdyas.org>
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

#include "TouchDockerDock.h"

#include <QtQuickWidgets/QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QAction>

#include <klocalizedstring.h>
#include <kactioncollection.h>

#include <KoResourcePaths.h>
#include <kis_icon.h>
#include <KoCanvasBase.h>
#include <KisViewManager.h>
#include <kis_canvas2.h>
#include <KisMainWindow.h>
#include <KisViewManager.h>

class TouchDockerDock::Private
{
public:
    Private()
    {
    }

    TouchDockerDock *q;
    bool allowClose {true};
    KisViewManager *viewManager {0};
    QString currentSketchPage;
};


TouchDockerDock::TouchDockerDock( )
    : QDockWidget(i18n("Touch Docker"))
    , d(new Private())
{
    m_quickWidget = new QQuickWidget(this);
    setWidget(m_quickWidget);
    setEnabled(true);
    m_quickWidget->engine()->rootContext()->setContextProperty("mainWindow", this);

    m_quickWidget->engine()->addImportPath(KoResourcePaths::getApplicationRoot() + "/lib/qml/");
    m_quickWidget->engine()->addImportPath(KoResourcePaths::getApplicationRoot() + "/lib64/qml/");

    m_quickWidget->setSource(QUrl("qrc:/kritasketch.qml"));
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

}

TouchDockerDock::~TouchDockerDock()
{
}

bool TouchDockerDock::allowClose() const
{
    return d->allowClose;
}

void TouchDockerDock::setAllowClose(bool allow)
{
    d->allowClose = allow;
}

QString TouchDockerDock::currentSketchPage() const
{
    return d->currentSketchPage;
}

void TouchDockerDock::setCurrentSketchPage(QString newPage)
{
    d->currentSketchPage = newPage;
    emit currentSketchPageChanged();
}

void TouchDockerDock::closeEvent(QCloseEvent* event)
{
    if (!d->allowClose) {
        event->ignore();
        emit closeRequested();
    } else {
        event->accept();
    }
}

QObject *TouchDockerDock::sketchKisView() const
{
    return d->viewManager;
}

void TouchDockerDock::setSketchKisView(QObject* newView)
{
    if (d->viewManager) {
        d->viewManager->disconnect(this);
    }

    if (d->viewManager != newView) {
        d->viewManager = qobject_cast<KisViewManager*>(newView);
        emit sketchKisViewChanged(); }
}

void TouchDockerDock::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(true);

    if (m_canvas == canvas) {
        return;
    }

    if (m_canvas) {
        m_canvas->disconnectCanvasObserver(this);
    }

    if (!canvas) {
        m_canvas = 0;
        return;
    }

    m_canvas = dynamic_cast<KisCanvas2*>(canvas);
}

void TouchDockerDock::unsetCanvas()
{
    setEnabled(true);
    m_canvas = 0;
}
