#pragma once

#include <QObject>
#include <QSystemTrayIcon>

class TurboButton : public QObject
{
    Q_OBJECT

public:
    TurboButton();

public slots:
    void togglePreference();

private:
    void updateIcon();

    QSystemTrayIcon *m_trayIcon;
};

