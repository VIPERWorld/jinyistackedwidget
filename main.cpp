#include <QtGui/QApplication>
#include "widget.h"
#include <QTextCodec>
#include<QWSServer>
#include <QFontDatabase>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    // QApplication::setOverrideCursor(Qt::BlankCursor);//隐藏鼠标 光标
     //QWSServer::setCursorVisible(false);

     //在开发板上 显示汉字
     #if 0
    // QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
     QFont font  = a.font();
     //font.setFamily("unifont");//wqy-zenhei
     font.setFamily("wqy-zenhei");//wqy-zenhei
     font.setPixelSize(16);
     a.setFont(font);
     #endif

     //在开发板上 显示汉字 添加文泉驿正黑字体
     QTextCodec *codec = QTextCodec::codecForName("UTF-8");
     QTextCodec::setCodecForCStrings(codec);
     QTextCodec::setCodecForTr(codec);
     QTextCodec::setCodecForLocale(codec);
     QFont font;
     font.setPixelSize(28);
     font.setFamily(("WenQuanYi Zen Hei"));
    
    return a.exec();
}
