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

private slots:
    void updateIcon();

private:
    QSystemTrayIcon *m_trayIcon;
};

