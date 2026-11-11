/**
 * @file labs/SignalSlotLab.h
 * @brief 信号槽连接类型验证窗口
 *
 * 职责：用最小可运行示例对比 Auto / Direct 两种连接在不同线程亲和性下
 * 的槽执行线程，作为跨线程通信行为的验证入口。
 *
 * 设计边界：Emitter / Receiver 为纯 QObject，不参与界面；
 * SignalSlotLab 只负责线程生命周期、连线与日志展示。
 */

#pragma once

#include <QWidget>
#include <QThread>

class QTextEdit;
class QPushButton;

/**
 * @class SignalEmitter
 * @brief 信号发射器，运行在主线程
 */
class SignalEmitter : public QObject
{
    Q_OBJECT
public:
    explicit SignalEmitter(QObject *parent = nullptr) : QObject(parent) {}

signals:
    /// 用 AutoConnection 连出去（跨线程时自动变成 Queued）
    void autoRequested(int value);
    /// 用 DirectConnection 连出去（即使跨线程也立即在发射线程执行）
    void directRequested(int value);
};

/**
 * @class SignalReceiver
 * @brief 接收器，会被 moveToThread 到子线程
 *
 * 约束：不能指定 parent。QObject::moveToThread 要求对象没有父对象，
 * 因为 Qt 规定「一个对象及其所有子对象必须属于同一线程」。
 */
class SignalReceiver : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

public slots:
    /// 打印当前槽到底跑在哪个线程
    void onValue(int value);
};

/**
 * @class SignalSlotLab
 * @brief 验证窗口：两个按钮 + 一个日志框
 */
class SignalSlotLab : public QWidget
{
    Q_OBJECT
public:
    explicit SignalSlotLab(QWidget *parent = nullptr);
    ~SignalSlotLab() override;

private slots:
    void onAutoClicked();
    void onDirectClicked();

private:
    /// 把一行文本追加到日志框（内部带线程 ID，方便对比）
    void appendLog(const QString &text);

private:
    QThread        m_workerThread;                  ///< 子线程对象，本身属于主线程
    SignalEmitter *m_emitter  = nullptr;            ///< 主线程对象，parent = this
    SignalReceiver* m_receiver = nullptr;           ///< 被移入子线程，无 parent
    QTextEdit     *m_log      = nullptr;
    QPushButton   *m_btnAuto  = nullptr;
    QPushButton   *m_btnDirect = nullptr;
    int            m_counter  = 0;
};
