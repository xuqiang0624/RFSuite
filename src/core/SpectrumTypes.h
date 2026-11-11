/**
 * @file core/SpectrumTypes.h
 * @brief 全局共享的数据结构：频谱帧、设备状态、报警记录
 *
 * 职责：定义「采集 → 缓冲 → 显示 → 存储 → 上报」数据流中各模块共用的
 * 数据契约，模块只依赖本文件，不互相依赖具体实现。
 *
 * 设计边界：
 *   1. 仅包含纯数据结构，不含 QObject、QWidget 与任何 IO。
 *   2. 字段对应一次扫频结果：中心频率、扫宽、点数、功率数组与峰值统计。
 *   3. 跨线程 Queued 连接传递的类型须 Q_DECLARE_METATYPE 并在入口注册，
 *      否则槽不会执行；QVector<double> 隐式共享，在子线程写副本会触发深拷贝。
 */

#pragma once

#include <QVector>
#include <QDateTime>
#include <QMetaType>
#include <QString>

/**
 * @struct SpectrumFrame
 * @brief 一帧扫频测量结果
 *
 * 频率轴约定：第 i 个点对应频率
 *     f(i) = (centerFreqMHz - spanMHz/2) + i * spanMHz / (N - 1)
 * 其中 N = powerDbm.size()。写成 freqAt() 是为了让绘图、光标读数、
 * 峰值标注三处共用同一套映射，避免各写一遍导致坐标对不上。
 */
struct SpectrumFrame
{
    qint64    sequence      = 0;        ///< 帧序号，用于统计丢帧与乱序
    QDateTime timestamp;                ///< 采集时刻（本地时间）
    double    centerFreqMHz = 1000.0;   ///< 中心频率，单位 MHz
    double    spanMHz       = 20.0;     ///< 扫宽，单位 MHz
    QVector<double> powerDbm;           ///< 功率数组，单位 dBm，长度即点数

    double peakDbm     = -120.0;        ///< 本帧最大功率
    double peakFreqMHz = 0.0;           ///< 峰值所在频率
    bool   alarm       = false;         ///< 是否触发阈值报警

    /// 频率轴起点（MHz）
    double startFreqMHz() const { return centerFreqMHz - spanMHz / 2.0; }
    /// 频率轴终点（MHz）
    double stopFreqMHz()  const { return centerFreqMHz + spanMHz / 2.0; }
    /// 点数
    int    pointCount()   const { return powerDbm.size(); }

    /// 第 i 个点对应的频率；N<=1 时退化为起点，避免除零
    double freqAt(int i) const
    {
        const int n = pointCount();
        if (n <= 1) return startFreqMHz();
        return startFreqMHz()
             + (stopFreqMHz() - startFreqMHz()) * double(i) / double(n - 1);
    }

    /// 是否是可绘制的合法帧
    bool isValid() const { return pointCount() >= 2; }
};

Q_DECLARE_METATYPE(SpectrumFrame)

/**
 * @enum DeviceState
 * @brief 设备连接状态机
 *
 * 用枚举统一描述连接状态迁移，避免散落的 bool 标志；
 * UI 状态栏按该枚举显示文字与颜色。
 */
enum class DeviceState
{
    Disconnected,   ///< 未连接
    Connecting,     ///< 正在建立连接
    Connected,      ///< 已连接且心跳正常
    Reconnecting,   ///< 断线重连中
    Timeout,        ///< 心跳超时
    Error           ///< 协议错误 / 不可恢复错误
};

Q_DECLARE_METATYPE(DeviceState)

/**
 * @struct AlarmRecord
 * @brief 一条阈值报警记录，与 SQLite 的 alarm 表字段一一对应
 */
struct AlarmRecord
{
    qint64    id = 0;
    QDateTime timestamp;
    double    peakDbm = 0.0;
    double    peakFreqMHz = 0.0;
    double    thresholdDbm = 0.0;
    QString   message;
};

Q_DECLARE_METATYPE(AlarmRecord)
