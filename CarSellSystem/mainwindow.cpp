#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include "donxml.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //开始时应该是车辆管理界面
    on_actioncar_triggered();
    //连接数据库
    connectDb();
    //初始化数据
    initData();
    //将数据库显示在view中
    showData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//连接数据库
void MainWindow::connectDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("root");
    db.setDatabaseName("car");

    //打开数据库
    if( !db.open() )
    {
        QMessageBox::warning(this,"数据库打开失败",db.lastError().text());
        return;
    }
}

//初始化
void MainWindow::initData()
{
    //通过模型来显示
    QSqlQueryModel *queryM = new QSqlQueryModel(this);  //创建模型
    queryM->setQuery("select name from factory");//sql语句
    ui->factory->setModel(queryM);  //给下拉框设置模型
    ui->boxInpotFactory->setModel(queryM);

    //设置当前页
    ui->toolBox->setCurrentIndex(0);

    //初始化数据
    ui->last->setText("0");
    ui->lineEditTotal->setEnabled(false);

    //创建空的xml
    donXML::createXML("../demo.xml");

}

//页面切换
//车辆管理 菜单
void MainWindow::on_actioncar_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->labeltext->setText("车辆管理");
}
//库存表 菜单
void MainWindow::on_actionstatistics_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->labeltext->setText("库存管理");
}

//车辆销售页面
//厂家下拉框槽函数
void MainWindow::on_factory_currentIndexChanged(const QString &arg1)
{
    if("请选择厂家" == arg1)
    {
        //内容清空
        ui->brand->clear();
        ui->linePrice->clear();
        ui->spinBox->setValue(0);
        ui->lineEditTotal->clear();
        ui->last->setText("0");

        ui->spinBox->setEnabled(false);
        ui->ButtonSure->setEnabled(false);

    }
    else
    {
        ui->brand->clear();

        QSqlQuery query;
        QString sql = QString("select name from brand where factory = '%1'").arg(arg1);

        //执行sql语句
        query.exec(sql);
        //获取内容
        while( query.next() )
        {
            QString name = query.value(0).toString();
            ui->brand->addItem(name);
        }

        //可以选择数量了
        ui->spinBox->setEnabled(true);
    }
}
//品牌下拉框
void MainWindow::on_brand_currentIndexChanged(const QString &arg1)
{
    //初始化 选择的数量
    ui->spinBox->setValue(0);

    QSqlQuery query;
    QString sql = QString("select price,last from brand "
                " where factory = '%1' and name = '%2'").arg(ui->factory->currentText()).arg(arg1);
    //执行sql语句
    query.exec(sql);
    //获取内容
    while( query.next() )
    {
        //价格
        int price = query.value("price").toInt();
        //剩余数
        last = query.value("last").toInt();         //定义全局变量 后面用方便

        //更新页面数据
        ui->linePrice->setText(QString::number(price));
        ui->last->setNum(last);
    }
}
//数据选择框槽函数
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    //更新剩余数量
    ui->ButtonSure->setEnabled(true);
    //设置选择范围
    ui->spinBox->setMaximum(last);

    ui->last->setNum( last - arg1 );

    //更新金额
    int price = ui->linePrice->text().toInt() * ui->spinBox->value();
    ui->lineEditTotal->setText(QString::number(price));
}
//确定槽函数
void MainWindow::on_ButtonSure_clicked()
{
    //按下确定更新数据库

    //获取销售数量
    int sell = ui->spinBox->value();
    //获取剩余的数量
    int last = ui->last->text().toInt();

    //获取数据库的销量
    QSqlQuery query;
    QString sql = QString("select sell from brand where factory ='%1' and name = '%2';")
            .arg(ui->factory->currentText()).arg(ui->brand->currentText());
    query.exec(sql);
    int DBsell = 0;

    while(query.next())
    {
        DBsell = query.value("sell").toInt();
    }

    DBsell += sell;

    QString str = QString("update brand set sell = '%1',last = '%2' "
                          "where factory ='%3' and name = '%4';")
            .arg(DBsell).arg(last).arg(ui->factory->currentText()).arg(ui->brand->currentText());
    query.exec(str);

    //把确认后的数据 ，更新到xml中
    QStringList list;
    list << ui->factory->currentText()
         << ui->brand->currentText()
         << ui->linePrice->text()
         << QString::number(sell)
         << ui->lineEditTotal->text();
    donXML::appendXML("../demo.xml",list);

    QStringList fList;
    QStringList bList;
    QStringList pList;
    QStringList nList;
    QStringList tList;
    QString date;
    donXML::readXML("../demo.xml",fList,bList,pList,nList,tList, date);

    ui->textEdit->setText(date);

    for(int i = 0 ; i < fList.size() ; i++)
    {
        QString str = QString("%1:%2:卖出了%3辆,单价：%4 总价：%5")
                .arg(fList.at(i))
                .arg(bList.at(i))
                .arg(nList.at(i))
                .arg(pList.at(i))
                .arg(tList.at(i));
        ui->textEdit->append(str);
    }

    //按完一次之后就让按钮不能再按
    ui->ButtonSure->setEnabled(false);
    on_ButtonCancel_clicked();//初始化一次界面

    //刷新显示数据表页面
    showData();
}
//取消槽函数
void MainWindow::on_ButtonCancel_clicked()
{
    //不对数据库进行操作   直接对页面复原就行
    ui->factory->setCurrentText("请选择厂家");
}

//车辆进货页面
//厂家下拉框槽函数
void MainWindow::on_boxInpotFactory_currentIndexChanged(const QString &arg1)
{
    if("请选择厂家" == arg1)
    {
        //内容清空
        ui->boxInpotBrand->clear();
        ui->spinBox_InputCount->setValue(0);

        //先屏蔽按钮
        ui->spinBox_InputCount->setEnabled(false);
        ui->buttonInputSure->setEnabled(false);

    }
    else
    {
        ui->boxInpotBrand->clear();

        QSqlQuery query;
        QString sql = QString("select name from brand where factory = '%1'").arg(arg1);

        //执行sql语句
        query.exec(sql);
        //获取内容
        while( query.next() )
        {
            QString name = query.value(0).toString();
            ui->boxInpotBrand->addItem(name);
        }

        //可以选择数量了
        ui->spinBox_InputCount->setEnabled(true);
    }
}
//品牌下拉框槽函数
void MainWindow::on_boxInpotBrand_currentIndexChanged(const QString &arg1)
{
    //初始化 选择的数量
    ui->spinBox_InputCount->setValue(0);

    QSqlQuery query;
    QString sql = QString("select sum,last from brand "
                " where factory = '%1' and name = '%2'").arg(ui->boxInpotFactory->currentText()).arg(arg1);
    //执行sql语句
    query.exec(sql);
    //获取内容
    while( query.next() )
    {
        //总数
        sum = query.value("sum").toInt();
        //剩余数
        last = query.value("last").toInt();         //定义全局变量 后面用方便
    }
}
//数据选择槽函数
void MainWindow::on_spinBox_InputCount_valueChanged(int arg1)
{
    //有数据后可以 确定了
    ui->buttonInputSure->setEnabled(true);
}
//确定槽函数
void MainWindow::on_buttonInputSure_clicked()
{
    last += ui->spinBox_InputCount->value();
    sum += ui->spinBox_InputCount->value();

    //更新数据库
    QSqlQuery query;
    QString sql = QString("update brand set sum = '%1',last = '%2' "
                          "where factory ='%3' and name = '%4';")
            .arg(sum).arg(last).arg(ui->boxInpotFactory->currentText()).arg(ui->boxInpotBrand->currentText());
    query.exec(sql);
    QString str = QString("进货，厂家%1，品牌%2，数量%3")
            .arg(ui->boxInpotFactory->currentText()).arg(ui->boxInpotBrand->currentText())
            .arg(ui->spinBox_InputCount->value());
    ui->textEdit->setText(str);

    //按完一次之后就让按钮不能再按
    ui->buttonInputSure->setEnabled(false);
    on_buttonInputCancel_clicked();//初始化一次界面
    //刷新显示数据表页面
    showData();
}
//取消槽函数
void MainWindow::on_buttonInputCancel_clicked()
{
    //不对数据库进行操作   直接对页面复原就行
    ui->boxInpotFactory->setCurrentText("请选择厂家");
}

//第二页数据库表
void MainWindow::showData()
{
    //设置模型
    model = new QSqlTableModel(this);
    model->setTable("brand");     //指定使用的表

    //把model 放到view中
    ui->tableView->setModel(model);
    //显示model里的数据
    model->select();        //可以在图形界面直接修改数据
    // 可以修改标头显示内容  第0个 水平方向 的修改为 序号
    model->setHeaderData(0,Qt::Horizontal,"序号");
    model->setHeaderData(1,Qt::Horizontal,"厂家");
    model->setHeaderData(2,Qt::Horizontal,"品牌");
    model->setHeaderData(3,Qt::Horizontal,"价格");
    model->setHeaderData(4,Qt::Horizontal,"总和");
    model->setHeaderData(5,Qt::Horizontal,"已卖出");
    model->setHeaderData(6,Qt::Horizontal,"剩余");
    //设置手动提交
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
}
//增加槽函数
void MainWindow::on_buttonAdd_clicked()
{
    //添加空记录（加一行空行）
    QSqlRecord record = model->record();//获取空记录
    //获取行号
    int row = model->rowCount();
    model->insertRecord(row,record);
}
//删除槽函数
void MainWindow::on_buttonDel_clicked()
{
    //取出选中部分 //获取选中的模型
    QItemSelectionModel *selectModel = ui->tableView->selectionModel();
    //取出模型中的索引
    QModelIndexList list = selectModel->selectedRows();
    //删除所有选中的行
    for(int i = 0; i < list.size();i++)
    {
        model->removeRow(list.at(i).row());
    }
}
//确定槽函数
void MainWindow::on_buttonSure_clicked()
{
    //提交
    model->submitAll();
}
//取消槽函数
void MainWindow::on_buttonCancel_clicked()
{
    //取消所有动作
    model->revertAll();
    //取消也是动作需要提交
    model->submit();
}



