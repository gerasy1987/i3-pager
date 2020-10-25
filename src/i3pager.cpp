#include "i3pager.h"

I3Pager::I3Pager(QObject* parent)
    : QObject(parent) {
    currentScreenPrivate = QString();
    mode = "default";

    qDebug() << "Starting i3 listener";
    this->i3ListenerThread = new I3ListenerThread(this);
    connect(i3ListenerThread, &I3ListenerThread::modeChanged, this, [=](const QString& mode) {
        this->mode = mode;
        Q_EMIT modeChanged();
    });
    connect(i3ListenerThread, &I3ListenerThread::workspacesChanged, this, [=]() {
        Q_EMIT workspacesChanged();
    });
    connect(i3ListenerThread, &I3ListenerThread::finished, i3ListenerThread, &QObject::deleteLater);
    i3ListenerThread->start();
    qDebug() << "i3 listener started";
}

I3Pager::~I3Pager() {
    qDebug() << "I3Pager destructor";
    this->i3ListenerThread->stop();
    this->i3ListenerThread->wait();
    qDebug() << "I3Pager destructor done";
}

QList<Workspace> I3Pager::getWorkspaces(bool filterByCurrentScreen) {
    QList<Workspace> workspaceList;
    try {
        i3ipc::connection conn;
        auto i3workspaceList = conn.get_workspaces();
        qInfo() << "Loading workspaces:";

        for (auto& i3workspace : i3workspaceList) {
            Workspace workspace;

            auto splitName = QString::fromStdString(i3workspace->name).split(':');
            workspace.id = QString::fromStdString(i3workspace->name);
            workspace.index = splitName[0];
            workspace.name = splitName.size() == 1 ? splitName[0] : splitName[1];
            workspace.icon = splitName.size() == 3 ? splitName[2] : "";

            workspace.output = QString::fromStdString(i3workspace->output);
            workspace.focused = i3workspace->focused;
            workspace.visible = i3workspace->visible;
            workspace.urgent = i3workspace->urgent;

            qInfo() << "i3Workspace name:" << QString::fromStdString(i3workspace->name);
            qInfo() << "Workspace:"
                    << "id:" << workspace.id
                    << "index:" << workspace.index
                    << "name:" << workspace.name
                    << "icon:" << workspace.icon
                    << "focused:" << workspace.focused
                    << "urgent:" << workspace.urgent
                    << "visible:" << workspace.visible
                    << "output:" << workspace.output;
            workspaceList.append(workspace);
        }
    } catch (...) {
        qWarning() << "i3ipc error";
    }

    if (filterByCurrentScreen) {
        workspaceList = Workspace::filterByCurrentScreen(workspaceList, this->currentScreenPrivate);
    }
    return workspaceList;
}

void I3Pager::activateWorkspace(QString workspace) {
    i3ipc::connection conn;
    conn.send_command("workspace " + workspace.toStdString());
}

void I3Pager::setCurrentScreen(QString screen) {
    this->currentScreenPrivate = screen;
    Q_EMIT workspacesChanged();
}

QString I3Pager::getMode() {
    return mode;
}

QString I3Pager::getCurrentScreen() {
    return this->currentScreenPrivate;
}
