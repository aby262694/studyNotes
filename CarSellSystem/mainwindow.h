#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //定义一个函数用来连接数据库
    void connectDb();
    //初始化数据
    void initData();
    void showData();


private slots:
    void on_actioncar_triggered();

    void on_actionstatistics_triggered();

    void on_factory_currentIndexChanged(const QString &arg1);

    void on_brand_currentIndexChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_ButtonSure_clicked();

    void on_ButtonCancel_clicked();

    void on_buttonAdd_clicked();

    void on_buttonDel_clicked();

    void on_buttonSure_clicked();

    void on_buttonCancel_clicked();

    void on_boxInpotFactory_currentIndexChanged(const QString &arg1);

    void on_boxInpotBrand_currentIndexChanged(const QString &arg1);

    void on_buttonInputSure_clicked();

    void on_buttonInputCancel_clicked();

    void on_spinBox_InputCount_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;

    //获取的数据库剩余数
    int last;
    //总数
    int sum;
    //显示model
    QSqlTableModel *model;
};
#endif // MAINWINDOW_H
