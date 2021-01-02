#ifndef DONXML_H
#define DONXML_H

#include<QString>
#include<QStringList>
#include<QDomDocument>
#include<QDomElement>

class donXML
{
public:
    donXML();

    //创建xml空文件
    static void createXML(QString filePath);

    static void appendXML(QString filePath,QStringList list);

    static void writeXML(QDomDocument& doc, QDomElement& root,QStringList &list);

    static void readXML(QString filePath, QStringList& fList,QStringList& bList,QStringList& pList,
                        QStringList& nList,QStringList& tList,QString& dateData);
};

#endif // DONXML_H
