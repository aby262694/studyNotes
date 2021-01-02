#include "donxml.h"

#include<QFile>
#include<QDomDocument>   //文件
#include<QDomProcessingInstruction>   //格式头
#include<QDomElement>    //元素

#include<QDebug>
#include<QTextStream>
#include<QDateTime>

#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

donXML::donXML()
{

}

void donXML::createXML(QString filePath)
{
    QFile file(filePath);
    //如果存在就不创建
    if(file.exists())
    {
        cout << "文件已经存在";
        return;
    }
    else
    {
        //不存在才创建
        if(file.open(QFile::WriteOnly))
        {
            //创建xml文档
            QDomDocument doc;
            //创建xml头部格式         <?xml version="1.0" encoding="UTF-8"?>
            QDomProcessingInstruction ins;
            ins = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
            //追加节点（元素）   就是把格式头放进去
            doc.appendChild(ins);
            //根节点元素
            QDomElement root = doc.createElement("日销售清单");
            doc.appendChild(root); //把根节点也放进去

            //保存
            QTextStream stream(&file);  //文本流关联文件
            doc.save(stream,4);  //参数2是缩进字符

            file.close();
        }
        else
        {
            cout << "writeonly error";
        }
    }
}
/***********************
 * 没有子节点创建<日期>子节点
 * 日期设置属性
 * 在日期创建时间
 * 时间设置属性
 * 时间下面创建<厂家>
 * 厂家设置内容
***********************/
void donXML::appendXML(QString filePath, QStringList list)
{
    QFile file(filePath);
    if( file.open(QFile::ReadOnly) )
    {
        //file 和 xml 文档对象关联
        QDomDocument doc;
        if(doc.setContent(&file))
        {
            file.close();  //相当于把文件里的东西读到了domDocument
            //获取根节点元素
            QDomElement root = doc.documentElement();

            //获取当前时间
            QDateTime date = QDateTime::currentDateTime();
            QString dateStr = date.toString("yyyy-MM-dd");

            //判断根节点下有没有子节点
            if(root.hasChildNodes())
            {
                //有子节点
                //查找最后一个子节点
                QDomElement lastEle = root.lastChildElement();
                //判断最后一个节点的日期是否为今天
                if(lastEle.attribute("date") == dateStr)
                {
                    //有当前日期
                    //直接写内容
                    writeXML(doc,lastEle,list);
                }
                else
                {
                    //没有当前日期
                    //创建日期节点
                    QDomElement dateEle = doc.createElement("日期");
                    //创建日期属性
                    QDomAttr dateAttr = doc.createAttribute("date");
                    //设置属性的值
                    dateAttr.setNodeValue(dateStr);
                    //节点设置属性  （把属性放给节点）
                    dateEle.setAttributeNode(dateAttr);

                    //把日期节点加在根节点上
                    root.appendChild(dateEle);

                    //写数据
                    writeXML(doc,dateEle,list);
                }

            }
            else
            {   //没有子节点
                //创建日期节点
                QDomElement dateEle = doc.createElement("日期");
                //创建日期属性
                QDomAttr dateAttr = doc.createAttribute("date");
                //设置属性的值
                dateAttr.setNodeValue(dateStr);
                //节点设置属性  （把属性放给节点）
                dateEle.setAttributeNode(dateAttr);

                //把日期节点加在根节点上
                root.appendChild(dateEle);

                //写数据
                writeXML(doc,dateEle,list);
            }
            //保存文件
            if( file.open(QFile::WriteOnly) )
            {
                QTextStream stream(&file);
                doc.save(stream,4);
                file.close();
            }
            else
            {
                cout << "writeOnly error";
                return;
            }

        }
        else
        {
            cout << "setContent error";
            return;
        }
    }
    else
    {
        cout << "ReadOnly error";
    }
}

void donXML::writeXML(QDomDocument &doc, QDomElement &root, QStringList &list)
{
    //获取当前时间
    QDateTime time = QDateTime::currentDateTime();
    QString timeStr = time.toString("hh:mm:dd");

    //创建时间节点元素
    QDomElement timeEle = doc.createElement("时间");
    QDomAttr timeAttr = doc.createAttribute("time");
    //给属性设定值
    timeAttr.setValue(timeStr);
    //时间节点和属性关联
    timeEle.setAttributeNode(timeAttr);
    //时间节点加到日期节点后面
    root.appendChild(timeEle);

    QDomElement factory = doc.createElement("厂家");
    QDomElement brand = doc.createElement("品牌");
    QDomElement price = doc.createElement("报价");
    QDomElement number = doc.createElement("数量");
    QDomElement total = doc.createElement("金额");

    //各个节点加内容
    QDomText text = doc.createTextNode(list.at(0));
    factory.appendChild(text);

    text = doc.createTextNode(list.at(1));
    brand.appendChild(text);

    text = doc.createTextNode(list.at(2));
    price.appendChild(text);

    text = doc.createTextNode(list.at(3));
    number.appendChild(text);

    text = doc.createTextNode(list.at(4));
    total.appendChild(text);

    //给时间节点加这5个节点
    timeEle.appendChild(factory);
    timeEle.appendChild(brand);
    timeEle.appendChild(price);
    timeEle.appendChild(number);
    timeEle.appendChild(total);

}

void donXML::readXML(QString filePath, QStringList &fList, QStringList &bList,
                     QStringList &pList, QStringList &nList, QStringList &tList,QString& dateData)
{
    QFile file(filePath);
    if(file.open(QFile::ReadOnly))
    {
        QDomDocument doc;
        if(doc.setContent(&file))
        {//关联成功可以 关闭文件了
            file.close();

            //获取根节点
            QDomElement root = doc.documentElement();
            QDateTime date = QDateTime::currentDateTime();
            QString dateStr = date.toString("yyyy-MM-dd");

            if(root.hasChildNodes())
            {   //有子节点
                QDomElement lastEle = root.lastChildElement();

                if(dateStr == lastEle.attribute("date"))
                {
                    dateData = dateStr;
                    //找出当前日期下所有的时间子节点
                    QDomNodeList list = lastEle.childNodes();
                    for(int i = 0 ; i< list.size() ; i++)
                    {
                        //将list中的点转换成元素   找到时间节点下的所有子节点
                        QDomNodeList subList = list.at(i).toElement().childNodes();
                        //取元素
                        fList << subList.at(0).toElement().text();
                        bList << subList.at(1).toElement().text();
                        pList << subList.at(2).toElement().text();
                        nList << subList.at(3).toElement().text();
                        tList << subList.at(4).toElement().text();
                    }

                }
                else
                {
                    cout << "没有当前日期";
                    return;
                }

            }
            else
            {   //没有子节点
                cout << "没有子节点";
                return;
            }
        }
        else
        {
            cout << "setContent error";
            return;
        }
    }
    else
    {
        cout << "readOnly error";
        return;
    }
}

