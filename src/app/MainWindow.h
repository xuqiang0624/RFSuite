/**
 * @file app/MainWindow.h
 * @brief 主窗口：承载界面装配与模块间接线。
 *
 * 设计边界：本类只负责窗口结构与信号槽连接，不实现采集、协议或存储逻辑；
 * 各功能以独立类接入，依赖方向保持单向向下。
 */

#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void buildUi();
};
