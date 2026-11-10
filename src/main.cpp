/**
 * @file main.cpp
 * @brief RFSuite 程序入口：初始化应用、装配可选模块、创建窗口并进入事件循环。
 *
 * 职责边界：入口只做装配，不承载业务逻辑；各模块的初始化与对象间连接
 * 由对应类负责，避免在入口堆积依赖。
 *
 * 可选模块通过 __has_include 在编译期探测：模块尚未加入工程时，相关
 * 初始化整段跳过，工程保持可构建；模块加入后自动启用，无需修改入口。
 *
 * 命令行：
 *   RFSuite                 启动主窗口
 *   RFSuite --lab=N         打开对应编号的独立验证窗口
 *   RFSuite --selftest=N    运行对应编号的自测并返回退出码
 */

#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QMainWindow>
#include <QWidget>

// 可选模块：头文件存在时才包含并启用对应初始化
#if __has_include("core/SpectrumTypes.h")
#include "core/SpectrumTypes.h"
#define RF_HAVE_TYPES 1
#endif
#if __has_include("core/Logger.h")
#include "core/Logger.h"
#define RF_HAVE_LOGGER 1
#endif
#if __has_include("core/AppConfig.h")
#include "core/AppConfig.h"
#define RF_HAVE_CONFIG 1
#endif
#if __has_include("core/Selftests.h")
#include "core/Selftests.h"
#define RF_HAVE_SELFTESTS 1
#endif
#if __has_include("app/MainWindow.h")
#include "app/MainWindow.h"
#define RF_HAVE_MAINWINDOW 1
#endif
#if __has_include("labs/SignalSlotLab.h")
#include "labs/SignalSlotLab.h"
#define RF_HAVE_SIGNALSLOT_LAB 1
#endif
#if __has_include("labs/ThreadLab.h")
#include "labs/ThreadLab.h"
#define RF_HAVE_THREAD_LAB 1
#endif
#if __has_include("labs/PaintLab.h")
#include "labs/PaintLab.h"
#define RF_HAVE_PAINT_LAB 1
#endif
#if __has_include("labs/EchoServer.h")
#include "labs/EchoServer.h"
#define RF_HAVE_DEVICE_LAB 1
#endif

/**
 * @brief 按编号创建独立验证窗口
 * @param n --lab= 后的编号
 * @return 未实现的编号返回 nullptr，由调用方回退到主窗口
 */
static QWidget *createLab(int n)
{
    switch (n) {
#ifdef RF_HAVE_SIGNALSLOT_LAB
    case 3:
        return new SignalSlotLab;
#endif
#ifdef RF_HAVE_THREAD_LAB
    case 5:
    case 8:
        return new ThreadLab;
#endif
#ifdef RF_HAVE_PAINT_LAB
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return new PaintLab;
#endif
#ifdef RF_HAVE_DEVICE_LAB
    case 15:
    case 16:
        return new DeviceLab;
#endif
    default:
        return nullptr;
    }
}

int main(int argc, char *argv[])
{
    // 1. 创建应用对象并设置应用元信息。
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("RFSuite"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));
    app.setOrganizationName(QStringLiteral("RFSuite"));

    // 2. 解析命令行开关；自测模块未接入时跳过对应参数。
    int labNo = 0;
#ifdef RF_HAVE_SELFTESTS
    int selftestNo = 0;
#endif
    for (int i = 1; i < argc; ++i) {
        const QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith(QStringLiteral("--lab=")))
            labNo = arg.mid(6).toInt();
#ifdef RF_HAVE_SELFTESTS
        else if (arg.startsWith(QStringLiteral("--selftest=")))
            selftestNo = arg.mid(11).toInt();
#endif
    }

#ifdef RF_HAVE_SELFTESTS
    // 3. 自测入口：不启动 GUI，直接返回退出码。
    if (selftestNo > 0)
        return runSelftest(selftestNo);
#endif

#ifdef RF_HAVE_TYPES
    // 4. 注册跨线程 Queued 连接所需的自定义元类型，未注册时槽不会执行。
    qRegisterMetaType<SpectrumFrame>("SpectrumFrame");
    qRegisterMetaType<DeviceState>("DeviceState");
    qRegisterMetaType<AlarmRecord>("AlarmRecord");
#endif

#ifdef RF_HAVE_LOGGER
    // 5. 安装全局消息处理器，统一收集运行日志。
    Logger::instance()->install();
#endif
#ifdef RF_HAVE_CONFIG
    // 6. 载入外部配置，业务代码不写死可变参数。
    AppConfig::instance()->load();
#endif

    // 7. 载入样式表；资源未接入时静默跳过。
    QFile qssFile(QStringLiteral(":/style.qss"));
    if (qssFile.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(QString::fromUtf8(qssFile.readAll()));
        qssFile.close();
    }

    // 8. 优先打开指定窗口，其次主窗口，最后占位窗口。
    QWidget *window = nullptr;
    if (labNo > 0)
        window = createLab(labNo);
#ifdef RF_HAVE_MAINWINDOW
    if (!window)
        window = new MainWindow;
#endif
    if (!window) {
        auto *placeholder = new QMainWindow;
        placeholder->setWindowTitle(QStringLiteral("RFSuite"));
        placeholder->resize(800, 600);
        auto *tip = new QLabel(QStringLiteral("主窗口模块尚未接入。"), placeholder);
        tip->setAlignment(Qt::AlignCenter);
        placeholder->setCentralWidget(tip);
        window = placeholder;
    }

    window->show();

    // 9. 进入事件循环，直到最后一个窗口关闭。
    return app.exec();
}
