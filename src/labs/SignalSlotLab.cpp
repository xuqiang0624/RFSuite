/**
 * @file labs/SignalSlotLab.cpp
 * @brief 信号槽连接类型验证窗口实现
 */

#include "labs/SignalSlotLab.h"

#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QDebug>

// 线程 ID 输出：QThread::currentThreadId() 返回平台相关类型，
// 先转 quintptr 再格式化，避免直接流式打印报错。
static QString currentThreadText()
{
    const auto id = reinterpret_cast<quintptr>(QThread::currentThreadId());
    return QStringLiteral("0x%1").arg(id, 0, 16);
}

void SignalReceiver::onValue(int value)
{
    qInfo().noquote() << QStringLiteral("[槽] value=%1 执行线程=%2")
                         .arg(value).arg(currentThreadText());
}

SignalSlotLab::SignalSlotLab(QWidget *parent)
    : QWidget(parent)
{
    // 1. 界面：两个按钮 + 一个只读日志框
    setWindowTitle(QStringLiteral("信号槽连接类型验证"));
    resize(720, 460);

    m_log = new QTextEdit(this);
    m_log->setReadOnly(true);

    m_btnAuto = new QPushButton(QStringLiteral("AutoConnection（跨线程→Queued）"), this);
    m_btnDirect = new QPushButton(QStringLiteral("DirectConnection（立即同步执行）"), this);

    auto *buttonRow = new QHBoxLayout;
    buttonRow->addWidget(m_btnAuto);
    buttonRow->addWidget(m_btnDirect);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonRow);
    mainLayout->addWidget(m_log);

    // 2. 创建接收器（无 parent）并移入子线程
    m_receiver = new SignalReceiver;
    m_receiver->moveToThread(&m_workerThread);

    // 3. 线程结束时释放接收器。deleteLater 是 deferred deletion：
    //    finished() 发出时事件循环已停，但延迟删除事件仍会被处理。
    connect(&m_workerThread, &QThread::finished,
            m_receiver, &QObject::deleteLater);

    // 4. 发射器属于主线程
    m_emitter = new SignalEmitter(this);

    // 5. 两条连接的差别只在第 5 个参数：
    //    Auto：发送者(主线程) 与 接收者(子线程) 亲和性不同 → 运行时按 Queued 投递，
    //          参数被拷贝进子线程事件队列，等子线程事件循环取出才执行。
    //    Direct：无视线程亲和性，在发射信号的那个线程里同步调用槽，
    //          所以日志里会看到「槽执行线程 == 主线程」。
    connect(m_emitter, &SignalEmitter::autoRequested,
            m_receiver, &SignalReceiver::onValue, Qt::AutoConnection);
    connect(m_emitter, &SignalEmitter::directRequested,
            m_receiver, &SignalReceiver::onValue, Qt::DirectConnection);

    connect(m_btnAuto,   &QPushButton::clicked, this, &SignalSlotLab::onAutoClicked);
    connect(m_btnDirect, &QPushButton::clicked, this, &SignalSlotLab::onDirectClicked);

    // 6. 必须先 start() 让子线程跑起事件循环，Queued 槽才有机会执行。
    m_workerThread.start();

    appendLog(QStringLiteral("主线程 = %1").arg(currentThreadText()));
    appendLog(QStringLiteral("子线程已启动。请分别触发两种连接，对比槽的执行线程。"));
    appendLog(QStringLiteral("预期：Auto 那条的槽线程 ID ≠ 主线程；Direct 那条 = 主线程。"));
}

SignalSlotLab::~SignalSlotLab()
{
    // 7. 退出顺序：quit 退出事件循环 → wait 确认线程结束，
    //    避免 QThread 在仍在运行时析构。
    m_workerThread.quit();
    m_workerThread.wait();
}

void SignalSlotLab::onAutoClicked()
{
    ++m_counter;
    appendLog(QStringLiteral("[发射 Auto] value=%1，发射线程=%2")
              .arg(m_counter).arg(currentThreadText()));
    emit m_emitter->autoRequested(m_counter);
    appendLog(QStringLiteral("Queued 槽在 emit 返回后执行，日志顺序可能倒置。"));
}

void SignalSlotLab::onDirectClicked()
{
    ++m_counter;
    appendLog(QStringLiteral("[发射 Direct] value=%1，发射线程=%2")
              .arg(m_counter).arg(currentThreadText()));
    emit m_emitter->directRequested(m_counter);
    appendLog(QStringLiteral("Direct 槽在 emit 内同步执行，日志顺序固定。"));
}

void SignalSlotLab::appendLog(const QString &text)
{
    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz"));
    m_log->append(QStringLiteral("[%1] %2").arg(stamp, text));
}
