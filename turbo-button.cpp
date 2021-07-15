#include "turbo-button.h"
#include "names.h"

#include <QFile>
#include <QIcon>
#include <QProcess>
#include <QDebug>
#include <QApplication>
#include <QTimer>

TurboButton::TurboButton() :
    QObject(qApp)
{
    m_trayIcon = new QSystemTrayIcon;
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &TurboButton::togglePreference);
    m_trayIcon->setIcon(QIcon::fromTheme("question"));
    updateIcon();
    m_trayIcon->show();

    // The preference is randomly changed behind our back, so set up a timer every minute to update
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->setTimerType(Qt::VeryCoarseTimer); // only second accuracy, but better for battery life
    timer->setInterval(60 * 1000); // every minute
    connect(timer, &QTimer::timeout, this, &TurboButton::updateIcon);
    timer->start();
}

void TurboButton::togglePreference()
{
    QProcess process;
    process.setProgram(LIBEXEC_DIR "/toggler");
    process.start();
    process.waitForFinished();
    if (process.exitCode() != 0) {
        qWarning() << "Failed to run toggler" << process.errorString() << process.exitCode();
    }
    updateIcon();
}

void TurboButton::updateIcon()
{
    QFile file(s_prefPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open preference" << file.errorString();
        togglePreference();
        return;
    }
    QByteArray preference = file.readAll();
    if (preference.endsWith(s_performance)) {
        m_trayIcon->setIcon(QIcon(":/fast.png"));
    } else if (preference.endsWith(s_power)) {
        m_trayIcon->setIcon(QIcon(":/slow.png"));
    } else {
        qWarning() << "Unknown preference" << preference;
        m_trayIcon->setIcon(QIcon::fromTheme("question"));
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    new TurboButton;
    return app.exec();
}
