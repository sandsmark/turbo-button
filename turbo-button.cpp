#include "turbo-button.h"
#include <QFile>
#include <QIcon>
#include <QProcess>
#include <QDebug>
#include <QApplication>
#include <QTimer>

static const char *s_prefFile = "/sys/devices/system/cpu/cpufreq/policy2/energy_performance_preference";
static const char *s_prefPerformance = "balance_performance\n";
static const char *s_prefPower = "balance_power\n";

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
    QFile file(s_prefFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open preference" << file.errorString();
        togglePreference();
        return;
    }
    QByteArray preference = file.readAll();
    if (preference.startsWith(s_prefPerformance)) {
        m_trayIcon->setIcon(QIcon(":/fast.png"));
    } else if (preference.startsWith(s_prefPower)) {
        m_trayIcon->setIcon(QIcon(":/slow.png"));
    } else {
        qWarning() << "Unknown preference";
        m_trayIcon->setIcon(QIcon::fromTheme("question"));
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    new TurboButton;
    return app.exec();
}
