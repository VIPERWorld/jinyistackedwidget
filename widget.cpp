#include "widget.h"
#include "ui_widget.h"

#include<QPainter>
#include<QPaintEvent>
#include<QTimer>
#include<QtSql>
#include<QTextCodec>
#include<QDebug>
#include<QMouseEvent>
#include<QString>
#include"hwlib/libshm.h"

/*******************************************************************************************************************/
//2017.1.14 wsj
//uchar flagFrame;
//界面选择标志
enum uiFlag
{
    xingZouWidget = 0,
    workWidget,
    setupWidget,
    timesetupwidget
};
uiFlag flagwidget;

//从setup 界面返回上一级界面标志
struct flagSwitch
{
    uchar flagwidget;
    uchar flagwork;
};
flagSwitch flagswitch;



RAM_CAN_sData cantest;
RAM_CAN_sData_ECU ecutest;

////时间设置 变量
QString  Datesetup;
bool timeSetOK = false; //时间设置完毕标志
QString DateTimeSetup;

////米计里程  蜂鸣器
//uchar flagMijLic = 0;//米计里程切换标志
//uchar flagbeep = 1;//蜂鸣器状态标志
//int lichengsum;//里程和，用于显示





long long xiaoshiJi_m;//分钟
double xiaoshiJi_h;//小时
float fadongzhuansu; //发动机转速


//uchar flagadd = 1;//转速方向标志位
uchar flagaddnum = 1;//转速方向标志位
uchar flagnum;
uchar flagDelay;//延时标志
uchar DelayCount;//延时六秒


uchar DelayYLcounter;//油量延时10秒响一次
uchar Delagbeepoff;//关之前延时
uchar flagbeepzero = 0;
//uchar flagYs;//延时2秒 防止一上电出现 蜂鸣器一直响。


int nu4;
uchar nu3;
float floatnu3;


uint nu2;
uint nu1;
float floatnu1;

//float nu4;
//float nu3;



int num = 0;
int numtmp = 0;

uint matchine[3] = {0}; //保存风机，复托器，升运器，等转速 的百分比
int ruby[3] = {0};
//频率量
ushort  shengyunqi =7;    //升运器
ushort  futuoqi =9;       //复脱器
ushort  zhouliuguntong=55;//轴流滚筒
ushort tuoliguntong=8;//脱粒滚筒转速


//米计 里程
//

double MIJILICHENG[3] = {0}; //米计 里程 状态标志
uint clearflag;//清零标志
uint  clearSave;//存储清零标志
uchar clearCount;

uint mijisum;//米计和， 用于显示
double licheng;//里程
int lichengsum;//里程和，用于显示

//水温 机油压力,电压数据
int NumSW;
float JYYL;
float DCDY;//电池电压

float YLBFB;//油量百分比

//闪烁标志
//
uchar shanshuoSW;//水温闪烁
uchar shanshuoJYYL;//机油压力闪烁
uchar shanshuoYL;//油量闪烁
uchar shanshuoYZYS;//油中有水
uchar shanshuoYYYW;//液压油温

uchar shanshuoLM;//粮满
uchar shanshuoSS;//手刹

uchar shanshuoTXGZ;//通信故障

uchar shanshuozlzs;//轴流滚筒转速
uchar shanshuoftqzs;//复托器转速
uchar shanshuosyqzs;//升运器转速



uchar flagMijLic = 0;//米计里程切换标志
uchar flagbeep = 1;//蜂鸣器状态标志


uchar j=0;
uchar jflag = 0;
uchar mm=0;
uchar jjjflag = 0;
unsigned char  mybufflag[15] = {0};
unsigned char  myindex[15] = {0};

/**************************/


/**************************************************************************************************************/
uint bjd[4] = {0};
//uint kaku_tlgt = 88;
//uint kaku_syq = 77;
bool flag_bjd = true;


bool flagjiajia = true;
int startflag = 0;

int  tuoliguntong_zs = 80; //脱离滚筒转速  //轴流滚筒
int fenliguntong_zs = 80;//分离滚筒
int fj_zs = 80;//风机
int ftq_zs = 80;//复脱器
int syq_zs = 80; //升运器
int qsq_zs = 80; //切碎器
int gq_zs = 80;//过桥转速



int kaku_tlgt;//脱离滚筒转速   //轴流滚筒
int kaku_flgt;//分离滚筒
int kaku_fj;//风机
int kaku_ftq;//复脱器
int kaku_syq;//升运器

int kaku_gqzs;//过桥转速


int ka_tlgt;//脱离滚筒转速
int ka_flgt;//分离滚筒
int ka_fj;//风机
int ka_ftq;//复脱器
int ka_syq;//升运器
int ka_qsq;//切碎器
int ka_gqzs;//过桥转速
/**************************************************************************************************************/

/*******************************************************************************************************************/

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //解析现象，当去掉 setWindowFlags(windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint); 这一句，在开发板子上能显示上方图标，但是不闪烁，
    //不注释则可以在板子上闪烁。
    setWindowFlags(windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint); //删除 最小化、最大化、关闭按钮

    timer_Main = new QTimer(this);   //声明一个定时器
    timer_xiaoshiji = new QTimer(); //声明小时计定时器
    timer_shanhua = new QTimer();
    timer_Licheng = new QTimer();//里程
    timergzm = new QTimer();//故障码

    connect(timergzm, SIGNAL(timeout()), this, SLOT(gzmslottest()));  //连接信号槽，定时器超时触发窗体更新
    connect(timer_Licheng,SIGNAL(timeout()),this,SLOT(Licheng()));//里程
    connect(timer_xiaoshiji,SIGNAL(timeout()),this,SLOT(xiaoshiji()));//小时计
    connect(timer_Main, SIGNAL(timeout()), this, SLOT(update()));  //连接信号槽，定时器超时触发窗体更新
    connect(timer_shanhua, SIGNAL(timeout()), this, SLOT(shanhua()));  //连接信号槽，定时器超时触发窗体更新

    timer_Main->start(30);   //启动定时器
    timer_xiaoshiji->start(1000);
    timer_Licheng->start(100);//里程
    timergzm->start(3000);//故障码
    timer_shanhua->start(30);

    Can_Ram_init();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *)
{
    //can 接收
    Can_Ram_Get(&cantest,&ecutest);



    QPainter painter(this);
    QPixmap pix;
    //图片选择
    switch(flagwidget)
    {
        case xingZouWidget:
        pix.load("./img2/xingzou.bmp");
        break;
        case workWidget:
        pix.load("./img2/work.bmp");
        break;
        case setupWidget:
        pix.load("./img2/setup.bmp");
        break;
//        case timesetupwidget:
//        break;
        default:
        break;
    }
    painter.drawPixmap(0,0,800,600,pix);

    if(flagwidget == xingZouWidget)
    {
        //int side = qMin(width(), height());  //绘制的范围(宽、高中最小值)

        painter.setRenderHint(QPainter::Antialiasing);//绘制的图像反锯齿
        //painter.translate(width() / 2, height() / 2);//重新定位坐标起始点，把坐标原点放到窗体的中央

        painter.translate(405, 284);//重新定位坐标起始点，把坐标原点放到窗体的中央
        //painter.scale(side / 400.0, side / 300.0);//设定画布的边界,用窗体宽高的最小值来计算时钟的大小，防止窗体拉伸导致的时钟变形以及显示不全

        //下面两个数组用来定义表针的两个顶点，以便后面的填充
        static const QPoint hourHand[4] = {
            QPoint(8, 0),
            QPoint(-8,0),
            QPoint(-1.5, -160),
            QPoint(1.5, -160)
        };
        static const QPoint minuteHand[4] = {
            QPoint(4, 0),
            QPoint(-4, 0),
            QPoint(-1, -90),
            QPoint(1, -90)
        };

        //km r/mini

        painter.setPen(Qt::NoPen);//填充时针，不需要边线所以NoPen
        painter.setBrush(Qt::red);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.save();
        //painter.rotate(6.0 *time.second());  //设旋转(角度 = 6° * (分钟 + 秒 / 60))
        painter.rotate(-120);
        //painter.rotate(6.0 *numtmp);  //设置旋转(6° * 秒)
        //painter.rotate(numtmp++);  //设置旋转(6° * 秒)
        //painter.rotate(4.0 );//*shisu

        //painter.rotate(8*nu1);//floatnu2
        painter.rotate(8*floatnu1);//floatnu1

        painter.drawConvexPolygon(hourHand, 4);  //填充分针部分
        painter.restore();

        /***********************************************/
        //2016.6.25   画白圈
        painter.setBrush(Qt::darkGray); //画上中心原点/home/vmuser/qtworkplace/chanpin/img2
        painter.save();
        painter.drawEllipse(QPoint(0,0),25,25);
        painter.restore();

        //画白圈
        /***********************************************/

        painter.setBrush(Qt::black);//画上中心原点/home/vmuser/qtworkplace/chanpin/img2
        painter.save();
        painter.drawEllipse(QPoint(0,0),20,20);
        painter.restore();


        //km/h
        painter.translate(0,148);//重新定位坐标起始点，把坐标原点放到窗体的中央
        //painter.scale(side / 400.0, side / 300.0);

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::red);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.save();

        //painter.rotate(6.0 *time.second());  //设旋转(角度 = 6° * (分钟 + 秒 / 60))
        painter.rotate(-120);
        //painter.rotate(8*nu3);
        painter.rotate(8*floatnu3);//


        //painter.rotate(4.0 );  //设旋转(角度 = 6° * (分钟 + 秒 / 60))*shisu
        //内测用
        /****************************************************************************/
        //qDebug()<<"time: "<<time.second()<<endl;

        /****************************************************************************/

        painter.drawConvexPolygon(minuteHand, 4);  //填充分针部分
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.restore();

        /***********************************************/
        //2016.6.25   画白圈
        painter.setBrush(Qt::darkGray); //画上中心原点/home/vmuser/qtworkplace/chanpin/img2
        painter.save();
        painter.drawEllipse(QPoint(0,0),13,13);
        painter.restore();

        //画白圈
        /***********************************************/


        painter.setBrush(Qt::black);
        painter.save();//画上中心原点
        painter.drawEllipse(QPoint(0,0),10,10);
        painter.restore();

        //chuli pupian shansuo
        painter.translate(-405,-432);//平移到左上角

    }



/*************************************************************************************************************/
     /*设置系统时间*/
     if(timeSetOK == true)
     {
     QString str1 = """";
     str1 += "date -s ";
     str1 += DateTimeSetup.left(10);
     str1 += """";

     //system("clock -w");
     //system("hwclock --hctosys");
     system(str1.toLatin1().data());

     //时间
     QString str2 = """";
     str2 += "date -s ";
     str2 += DateTimeSetup.mid(11,9);
     str2 += """";
     system(str2.toLatin1().data());
     system("hwclock -w");
     timeSetOK = false;
    }

//
//控件显示
//显示时间 。2017.1.14
QString timeStr1= QTime::currentTime().toString();     //绘制当前的时间
QString dateStr1 = QDate::currentDate().toString("yyyy-MM-dd");

//不同界面显示时间
if(flagwidget == xingZouWidget)
{
    ui->label_31->setText(dateStr1);//dateStr1
    ui->label_35->setText(timeStr1);
}
else if(flagwidget == workWidget)
{
    ui->label_15->setText(dateStr1);//dateStr1
    ui->label_16->setText(timeStr1);
}
else if(flagwidget == setupWidget)
{
    ui->label_52->setText(dateStr1);//dateStr1
    ui->label_51->setText(timeStr1);
}
/**************************************************************************************************************/
//共有部分

QPainter paintShanshuo_W(this);
QPixmap pixShanshuo_W;

#if 1

           if(shanshuoSW)//水温 0~120度  95度以上报警。
           {
                pixShanshuo_W.load("./img2/icon/09.png");//07.jpg
                paintShanshuo_W.drawPixmap(381,6,31,28,pixShanshuo_W);//正上方位置显示的图标
           }

           if(shanshuoJYYL) //机油  油压报警 0～1 MPa,在0.1 MPa 以下为报警区。
           {
                pixShanshuo_W.load("./img2/icon/12.png");//08.jpg
                paintShanshuo_W.drawPixmap(496,10,48,20,pixShanshuo_W);//正上方位置显示的图标
                //paintShanshuo_W.drawPixmap(20,180,43,43,pixShanshuo_W);//左边显示的图标
           }


           if (shanshuoYL)//油量
           {
               pixShanshuo_W.load("./img2/icon/10.png");//14.jpg
               paintShanshuo_W.drawPixmap(420,6,24,28,pixShanshuo_W);

           }

           //油中右水  油水分离  需要闪烁
           if(shanshuoYZYS)
           {
               pixShanshuo_W.load("./img2/icon/16.png");//14.jpg
               paintShanshuo_W.drawPixmap(680,5,29,31,pixShanshuo_W);
           }

           //液压油温
           if(shanshuoYYYW)//液压油温  cantest.VolYeyayouwen
           {
                pixShanshuo_W.load("./img2/icon/89.png");//10.jpg
                paintShanshuo_W.drawPixmap(338,7,34,27,pixShanshuo_W);//上边图标
                //paintShanshuo_W.drawPixmap(423,216,49,38,pixShanshuo_W);//左边图标
           }

    //闪烁 转速

#if 1
    if(shanshuozlzs)//轴流滚筒
    {
        pixShanshuo_W.load("./img2/icon/37.png");//11.   jpgenglish/shanshuo/
        paintShanshuo_W.drawPixmap(631,126,117,24,pixShanshuo_W);
    }

    if(shanshuoftqzs)//复脱器
    {
        pixShanshuo_W.load("./img2/icon/38.png");//11.jpg
        paintShanshuo_W.drawPixmap(631,215,99,24,pixShanshuo_W);
    }

    if(shanshuosyqzs)//升运器
    {
        pixShanshuo_W.load("./img2/icon/39.png");//11.jpg
        paintShanshuo_W.drawPixmap(631,305,101,24,pixShanshuo_W);
    }

#endif
   //不用闪烁

   if(cantest.flagKD)//空档
   {
       pixShanshuo_W.load("./img2/icon/60.png");//14.jpg
       paintShanshuo_W.drawPixmap(710,50,50,50,pixShanshuo_W);
   }
   else if((cantest.flagKD == 0)&&(cantest.flagDC == 0))//前进档位
   {
       pixShanshuo_W.load("./img2/icon/59.png");//14.jpg
       paintShanshuo_W.drawPixmap(710,50,50,50,pixShanshuo_W);
   }


   if(cantest.flagLeft)//左转灯闪烁cantest.flagLeft
   {
       pixShanshuo_W.load("./img2/icon/01.png");//14.jpg
       paintShanshuo_W.drawPixmap(9,3,33,34,pixShanshuo_W);
   }
   if(cantest.flagBattery)//电瓶指示灯  flagBattery
   {
       pixShanshuo_W.load("./img2/icon/05.png");//14.jpg
       paintShanshuo_W.drawPixmap(190,8,34,24,pixShanshuo_W);//正上方图片显示
       //paintShanshuo_W.drawPixmap(20,335,43,34,pixShanshuo_W);//左边图片显示
   }
   //空滤
   if(cantest.flagKL)
   {
       pixShanshuo_W.load("./img2/icon/20.png");//04.jpg
       paintShanshuo_W.drawPixmap(52,4,32,32,pixShanshuo_W);
   }
   //粮满
   if(cantest.flagLCM)
   {
       pixShanshuo_W.load("./img2/icon/15.png");//13.jpg
       paintShanshuo_W.drawPixmap(641,4,32,32,pixShanshuo_W);
   }

//            if(flagLCM_seven)//flagLCM = 1; //粮仓满7000000000
//            {
//                pixShanshuo_W.load("./imagezl/66/70.png");//13.jpg
//                paintShanshuo_W.drawPixmap(542,15,42,41,pixShanshuo_W);
//            }

   if(ecutest.flagFDJYR_ECU)//flagFDJYR = 1; //发动机预热
   {
       pixShanshuo_W.load("./img2/icon/11.png");//15.jpg
       paintShanshuo_W.drawPixmap(451,8,37,23,pixShanshuo_W);
   }



   //ECU
   if(ecutest.flagECU)
   {
       pixShanshuo_W.load("./img2/icon/07.png");//03.jpg
       paintShanshuo_W.drawPixmap(290,9,42,21,pixShanshuo_W);
   }

   if(ecutest.flagFDJGZ_ECU)//发动机故障cantest.flagFDJGZ
   {
       pixShanshuo_W.load("./img2/icon/14.png");//16.jpg
       paintShanshuo_W.drawPixmap(597,9,36,23,pixShanshuo_W);
   }


   if(cantest.flagWidthlamp)//示宽灯
   {
       pixShanshuo_W.load("./img2/icon/04.png");//06.jpg
       paintShanshuo_W.drawPixmap(139,7,43,24,pixShanshuo_W);
   }

   if(cantest.flagJG)//近光灯flagJG
   {
       pixShanshuo_W.load("./img2/icon/03.png");//12.jpg
       paintShanshuo_W.drawPixmap(95,6,37,27,pixShanshuo_W);
   }

   if(cantest.flagYG)//远光灯
   {
       pixShanshuo_W.load("./img2/icon/02.png");//05.jpg
       paintShanshuo_W.drawPixmap(714,9,38,23,pixShanshuo_W);
   }

   if(cantest.flagSS)//停车 刹车
   {
       pixShanshuo_W.load("./img2/icon/13.png");//11.jpg
       paintShanshuo_W.drawPixmap(552,6,37,29,pixShanshuo_W);
   }

   if(cantest.flagRight)//右转
   {
       pixShanshuo_W.load("./img2/icon/17.png");//14.jpg
       paintShanshuo_W.drawPixmap(758,2,34,37,pixShanshuo_W);
   }

   //MCU
   if(cantest.flagTXCS)
   {
       pixShanshuo_W.load("./img2/icon/06.png");//14.jpg
       paintShanshuo_W.drawPixmap(232,8,49,21,pixShanshuo_W);
   }

#endif

/**************************************************************************************************************/

/**************************************************************************************************************/
/**************************************************************************************************************/
//
//控件显示
//显示 米计里程



//lichengsum = cantest.VolLICHENG;
//lichengsum /= 10;

//用于显示米 千米
QPainter paintShanshuo_WMIKM(this);
QPixmap pixShanshuo_WMIKM;

if (flagMijLic)// flagMijLic = 0； 显示米计
{
    if(flagwidget == xingZouWidget)
    {
        ui->pushButton_10->setStyleSheet("QPushButton{border-image:url(./img2/icon/36.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
        //ui->label_12->setText(QString::number(cantest.VolMIJISUM));//米计

        //显示米计   m 图标
        pixShanshuo_WMIKM.load("./img2/icon/444.png");//14.jpg
        paintShanshuo_WMIKM.drawPixmap(760,450,25,25,pixShanshuo_WMIKM);
    }
    else if(flagwidget == workWidget)
    {
        ui->pushButton_11->setStyleSheet("QPushButton{border-image:url(./img2/icon/36.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
        //ui->label_12->setText(QString::number(cantest.VolMIJISUM));//米计

        //显示米计   m 图标
        pixShanshuo_WMIKM.load("./img2/icon/444.png");//14.jpg
        paintShanshuo_WMIKM.drawPixmap(760,450,25,25,pixShanshuo_WMIKM);
    }
}
else //显示 里程
{
    if(flagwidget == xingZouWidget)
    {
        ui->pushButton_10->setStyleSheet("QPushButton{border-image:url(./img2/icon/35.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
        ui->label_12->setText(QString::number(lichengsum));//里程
        //显示 里程 km 图标
        pixShanshuo_W.load("./img2/icon/333.png");//14.jpg
        paintShanshuo_W.drawPixmap(760,450,25,25,pixShanshuo_W);
    }
    else if(flagwidget == workWidget)
    {
        ui->pushButton_11->setStyleSheet("QPushButton{border-image:url(./img2/icon/35.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
        ui->label_12->setText(QString::number(lichengsum));//里程
        //显示 里程 km 图标
        pixShanshuo_W.load("./img2/icon/333.png");//14.jpg
        paintShanshuo_W.drawPixmap(760,450,25,25,pixShanshuo_W);
    }
}


/**************************************************************************************************************/
//
//控件显示
//显示 蜂鸣器
if (flagbeep)// flagbeep = 0； 喇叭未摁下
{
    if(flagwidget == xingZouWidget)
    {
        ui->pushButton_3->setStyleSheet("QPushButton{border-image:url(./img2/beeplog.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");

    }else if (flagwidget == workWidget)
    {
        ui->pushButton_6->setStyleSheet("QPushButton{border-image:url(./img2/beeplog.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
    }
}
else //显示 喇叭摁下
{
    if(flagwidget == xingZouWidget)
    {
        ui->pushButton_3->setStyleSheet("QPushButton{border-image:url(./img2/nobeep.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");

    }
    else if(flagwidget == workWidget)
    {
        ui->pushButton_6->setStyleSheet("QPushButton{border-image:url(./img2/nobeep.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
    }
}

/****************************************************************************************************************/
//控件显示
//显示 油量，机油压力，水温，电池，FMI,SPN，轴流滚筒，复脱器，升运器
/**************************************************************************************************************/




// setup界面 设置滚筒转速 并实时显示

if(flagwidget == setupWidget)
{
#if 1

     /***********************************************************************************/
     //读取报警点(报警点 == 一键设定值*打滑率)
     /***********************************************************************************/
     //读取数据库信息
     //

     QTextCodec::setCodecForTr(QTextCodec::codecForLocale());//汉字显示

     QSqlDatabase db;
     if(QSqlDatabase::contains("qt_sql_default_connection"))
       db = QSqlDatabase::database("qt_sql_default_connection");
     else
       db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("jy.db");
    if (!db.open())
    {
        qDebug()<<"open database failed ---"<<db.lastError().text()<<"/n";
    }
    QSqlQuery query;
 //   bool ok = query.exec("CREATE TABLE IF NOT EXISTS  myjy10 (id INTEGER PRIMARY KEY AUTOINCREMENT,"
 //                                      "name VARCHAR(20) NOT NULL,"
 //                                      "age INTEGER NULL)");
 #if 0
    bool ok = query.exec("create table myjy10(id INTEGER,name varchar,age INTEGER)");
    if (ok)
    {
        qDebug()<<"ceate table partition success"<<endl;
    }
    else
    {
        qDebug()<<"ceate table partition failed"<<endl;
    }
 #endif
         //query.prepare("INSERT INTO myjy10 (id, name, age) VALUES (:id, :name, :age)");

         uchar i = 0;
         query.exec("select id, name, age from jy8c");
         while (query.next())
         {

            //qDebug()<<"id("<<query.value(0).toInt()<<")  name:"<<query.value(1).toString()<<"  age:"<<query.value(2).toInt();
             //qDebug()<<query.value(2).toInt();

             bjd[i++] = query.value(2).toInt();

             //qDebug()<<"opqrst"<<endl;
         }

         ka_tlgt = bjd[0];
         ka_ftq  = bjd[1];
         ka_syq  = bjd[2];

         startflag = bjd[3];

         if (flag_bjd)
         {
             ui->label_45->setText(QString::number(bjd[0],10));
             ui->label_43->setText(QString::number(bjd[1],10));
             ui->label_44->setText(QString::number(bjd[2],10));

             kaku_tlgt = bjd[0];
             kaku_ftq = bjd[1];
             kaku_syq = bjd[2];

         }
         else
         {

              ui->label_45->setText(QString::number(kaku_tlgt,10));//(ui->label->text().toInt()
              ui->label_43->setText(QString::number(kaku_ftq,10));//kaku_qsq

              ui->label_44->setText(QString::number(kaku_syq,10));//kaku_qsq
         }

         query.exec(QObject::tr("drop jy8c"));
#endif
} //endof  if(flagwidget == setupWidget)

/***************************************************************************************************************/
//读取滚筒转速

/***************************************************************************************************************************************************/
//读取数据库信息
//
        QTextCodec::setCodecForTr(QTextCodec::codecForLocale());//汉字显示

        QSqlDatabase db;
        if(QSqlDatabase::contains("qt_sql_default_connection"))
          db = QSqlDatabase::database("qt_sql_default_connection");
        else
          db = QSqlDatabase::addDatabase("QSQLITE");

       db.setDatabaseName("jy.db");
       if (!db.open())
       {
           qDebug()<<"open database failed ---"<<db.lastError().text()<<"/n";
       }
       QSqlQuery query;

    #if 0
       bool ok = query.exec("create table myjy10(id INTEGER,name varchar,age INTEGER)");
       if (ok)
       {
           qDebug()<<"ceate table partition success"<<endl;
       }
       else
       {
           qDebug()<<"ceate table partition failed"<<endl;
       }
    #endif
            //query.prepare("INSERT INTO myjy10 (id, name, age) VALUES (:id, :name, :age)");

            uchar i = 0;
            query.exec("select id, name, age from jy8c");
            while (query.next())
            {
                matchine[i++] = query.value(2).toInt();
            }

    #if 1
            i = 0;
            for (i = 0; i < 3; i++)
                {
                    //qDebug()<<"matchine % bi "<<matchine[i]<<endl;
                }
            i = 0;
    #endif



            query.exec(QObject::tr("drop jy8c"));

    /*******************************************************************/

                   ruby[0] = matchine[0];
                   ruby[1] = matchine[1];
                   ruby[2] = matchine[2];

                  //比较can传过来的值 ，实现闪烁

#if 0
              //if(can_retern != 0)
              {
                  //qDebug()<<"return != 0"<<endl;

                   if(tuoliguntong<ruby[0])//轴流滚筒转速
                   {
                       if (tuoliguntong != 0)
                       {
                           pixShanshuo_W.load("./img2/icon/37.png");//11.   jpgenglish/shanshuo/
                           paintShanshuo_W.drawPixmap(631,126,117,24,pixShanshuo_W);
                       }

                       // qDebug()<<"bpj"<<endl;
                   }

                   if(futuoqi<ruby[1])//复脱器转速
                   {
                       if (futuoqi != 0)
                       {
                           pixShanshuo_W.load("./img2/icon/38.png");//11.jpg
                           paintShanshuo_W.drawPixmap(631,215,99,24,pixShanshuo_W);
                       }

                       //qDebug()<<"syqzs"<<endl;
                   }

                   if(shengyunqi<ruby[2])//升运器堵塞转速
                   {
                       if (shengyunqi != 0)
                       {
                           pixShanshuo_W.load("./img2/icon/39.png");//11.jpg
                           paintShanshuo_W.drawPixmap(631,305,101,24,pixShanshuo_W);
                       }

                       //qDebug()<<"syqzs"<<endl;
                   }

              }//endof if(can_return != 0)
#endif


/***************************************************************************************************************/
// label 显示
#if 1
    if(flagwidget == xingZouWidget)
    {
        //控件显示
        //显示 油量，机油压力，水温，电池，FMI,SPN，轴流滚筒，复脱器，升运器
        ui->label_34->setText(QString::number(ecutest.VolSW));//水温
        //油量
        YLBFB = cantest.VolYL;
        YLBFB *= 12.5;

        if(YLBFB > 100)
        {
            ui->label_42->setText(QString::number(100));
        }
        else
        {
            ui->label_42->setText(QString::number(YLBFB));
        }
        //电池电压
        DCDY =cantest.VOLV;
        DCDY /= 10;
        ui->label_41->setText(QString::number(DCDY,'f',1));//电池电压

        JYYL = ecutest.VolJYYL;
        JYYL = JYYL/1000;
        ui->label_40->setText(QString::number(JYYL,'f',2));//机油压力

        ui->label_39->setText(QString::number(cantest.FTspeed));//复脱器
        ui->label_37->setText(QString::number(cantest.SYspeed));//升运器
        ui->label_30->setText(QString::number(cantest.ZLspeed));//轴流滚筒
    }
    else if (flagwidget == workWidget)
    {
        //控件显示
        //显示 油量，机油压力，水温，电池，FMI,SPN，轴流滚筒，复脱器，升运器
        ui->label_3->setText(QString::number(ecutest.VolSW));//水温
        //油量
        YLBFB = cantest.VolYL;
        YLBFB *= 12.5;

        if(YLBFB > 100)
        {
            ui->label->setText(QString::number(100));
        }
        else
        {
            ui->label->setText(QString::number(YLBFB));
        }
        //电池电压
        DCDY =cantest.VOLV;
        DCDY /= 10;
        ui->label_4->setText(QString::number(DCDY,'f',1));//电池电压

        JYYL = ecutest.VolJYYL;
        JYYL = JYYL/1000;
        ui->label_2->setText(QString::number(JYYL,'f',2));//机油压力

        ui->label_9->setText(QString::number(cantest.FTspeed));//复脱器
        ui->label_10->setText(QString::number(cantest.SYspeed));//升运器
        ui->label_8->setText(QString::number(cantest.ZLspeed));//轴流滚筒

        //发动机转速
        ui->label_50->setText(QString::number(ecutest.FDJ_speed));// r/min

        float floatworknu4 = (cantest.HourSpeed);///100
        floatworknu4 /= 10;

        //时速
        ui->label_49->setText(QString::number(floatworknu4,'f',1));// km/h
    }
#endif





}//end of void Widget::paintEvent(QPaintEvent *)


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
//widget界面 button

void Widget::on_pushButton_clicked() //从widget 界面到 work 界面的button
{
    flagwidget = workWidget;
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_pushButton_2_clicked() //从widget 界面到 setup 界面
{
    flagwidget = setupWidget;
    flagswitch.flagwidget = 1;
    ui->stackedWidget->setCurrentIndex(2);
}

void Widget::on_pushButton_3_clicked()//widget 界面 蜂鸣器
{
    //控件显示
    //显示 蜂鸣器
    flagbeep ^= 1;
    if (flagbeep)// flagbeep = 0； 喇叭未摁下
    {
        ui->pushButton_3->setStyleSheet("QPushButton{border-image:url(./img2/beeplog.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
    }
    else //显示 喇叭摁下
    {
        beep_off();
        ui->pushButton_3->setStyleSheet("QPushButton{border-image:url(./img2/nobeep.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
    }
}


//work  界面 button

void Widget::on_pushButton_4_clicked()//从work 界面 返回到 行走界面
{
    flagwidget = xingZouWidget;
    ui->stackedWidget->setCurrentIndex(0);
}

void Widget::on_pushButton_5_clicked()//从work界面进入到设置界面
{
   flagwidget = setupWidget;
   flagswitch.flagwork = 1;
   ui->stackedWidget->setCurrentIndex(2);
}

void Widget::on_pushButton_6_clicked()//work 界面 蜂鸣器
{
    //控件显示
    //显示 蜂鸣器
    flagbeep ^= 1;
    if (flagbeep)// flagbeep = 0； 喇叭未摁下
    {
        ui->pushButton_6->setStyleSheet("QPushButton{border-image:url(./img2/beeplog.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
    }
    else //显示 喇叭摁下
    {
        beep_off();
        ui->pushButton_6->setStyleSheet("QPushButton{border-image:url(./img2/nobeep.png);background-repeat: background-xy;background-position: center;background-attachment: fixed;background-clip: padding}");
    }
}


//setup 界面 button

void Widget::on_pushButton_7_clicked()//从setup 界面 to widget 或 work   //数据存储
{
/************************************************************************************************************/
    //ui->pushButton_16->setStyleSheet("QPushButton{border-image:url(./imagejy/yjsd.png);background-repeat: repeat-xy;background-position: center;background-attachment: fixed;background-clip: padding}");

       //system("rm my5.db");
       QTextCodec::setCodecForTr(QTextCodec::codecForLocale());//汉字显示

       QSqlDatabase db;
       if(QSqlDatabase::contains("qt_sql_default_connection"))
         db = QSqlDatabase::database("qt_sql_default_connection");
       else
         db = QSqlDatabase::addDatabase("QSQLITE");

   //   QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName("jy.db");
      if (!db.open())
      {
          qDebug()<<"open database failed ---"<<db.lastError().text()<<"/n";
      }
      QSqlQuery query;


   #if 0 //创建数据库
      bool ok = query.exec("create table jy8c(id INTEGER,name varchar,age INTEGER)");
      if (ok)
      {
          qDebug()<<"ceate table partition success"<<endl;
      }
      else
      {
          qDebug()<<"ceate table partition failed"<<endl;
      }
   #endif
             //query.prepare("INSERT INTO jy8c (id, name, age) VALUES (:id, :name, :age)");

             query.prepare("update jy8c set age = :age where id = :id");


             qDebug()<<"kakutlgt ---  "<<kaku_tlgt<<endl;
            qDebug()<<"syq ----- ---   "<<kaku_syq<<endl;


             query.bindValue(":id",1);
             //query.bindValue(":name", QObject::tr("轴流转速"));
             query.bindValue(":age", kaku_tlgt);//ui->label->text().toInt()
             query.exec();

             query.bindValue(":id",2);
             //query.bindValue(":name", QObject::tr("复脱塞转速"));
             query.bindValue(":age", kaku_ftq);//ui->label_3->text().toInt()
             query.exec();


             query.bindValue(":id",3);
             //query.bindValue(":name", QObject::tr("升运转速"));
             query.bindValue(":age", kaku_syq);//ui->label_3->text().toInt()
             query.exec();

             query.bindValue(":id",8);
             //query.bindValue(":name", QObject::tr("初始化标志"));
             query.bindValue(":age", 1);
             query.exec();

           query.exec("select id, name, age from jy8c");
           while (query.next())
           {

              qDebug()<<"id("<<query.value(0).toInt()<<")  name:"<<query.value(1).toString()<<"  age:"<<query.value(2).toInt();
           }

           query.exec(QObject::tr("drop jy8c"));

           //ui->label->setText(QString::number(bjd[0],10));

           flag_bjd = true;

           //flag_bjd2 = true;

/************************************************************************************************************/
    if(flagswitch.flagwidget)
    {
        flagwidget = xingZouWidget;
        flagswitch.flagwidget = 0;
        ui->stackedWidget->setCurrentIndex(0);
    }
    else if(flagswitch.flagwork)
    {
        flagwidget = workWidget;
        flagswitch.flagwork = 0;
        ui->stackedWidget->setCurrentIndex(1);
    }  
}

void Widget::on_pushButton_8_clicked() //从设置界面 进入到时间设置界面
{
     flagwidget = timesetupwidget;
     ui->stackedWidget->setCurrentIndex(3);
}



//米计里程
//widget 里程按钮 米计切换按钮
void Widget::on_pushButton_10_clicked()
{
    flagMijLic ^= 1;
    if (flagMijLic)// flagMijLic = 0； 显示米计
    {
        //Can_set_mijiclr_flag(1);//发送清零标志
        ui->label_32->setText(QString::number(10));//米计 cantest.VolMIJISUM
    }
    else //显示 里程
    {
        ui->label_32->setText(QString::number(lichengsum));//里程
    }
}

//work  里程按钮 米计切换按钮
void Widget::on_pushButton_11_clicked()
{
    flagMijLic ^= 1;
    if (flagMijLic)// flagMijLic = 0； 显示米计
    {
        //Can_set_mijiclr_flag(1);//发送清零标志
        ui->label_12->setText(QString::number(99));//米计 cantest.VolMIJISUM
    }
    else //显示 里程
    {
        ui->label_12->setText(QString::number(lichengsum));//里程
    }
}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

/*############################################################################################################*/
//时间设置按钮
//timesetup 界面 button

void Widget::on_pushButton_32_clicked()//关闭按钮  //从时间设置 界面 返回设置界面
{
    flagwidget = setupWidget;
    ui->stackedWidget->setCurrentIndex(2);
}

void Widget::on_pushButton_22_clicked() //按键 1
{
    DateTimeSetup.append("1");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_19_clicked() //按键 2
{
    DateTimeSetup.append("2");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_33_clicked()//按键 3
{
    DateTimeSetup.append("3");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_29_clicked()//按键 4
{
    DateTimeSetup.append("4");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_28_clicked()//按键 5
{
    DateTimeSetup.append("5");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_27_clicked()//按键 6
{
    DateTimeSetup.append("6");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_18_clicked()//按键 7
{
    DateTimeSetup.append("7");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_23_clicked()//按键 8
{
    DateTimeSetup.append("8");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_21_clicked()//按键 9
{
    DateTimeSetup.append("9");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_24_clicked()//按键 0
{
    DateTimeSetup.append("0");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_25_clicked()//按键 esc
{
    DateTimeSetup = DateTimeSetup.left(DateTimeSetup.length() - 1);
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_20_clicked()//按键 ok
{
    timeSetOK = true;
    flagwidget = setupWidget;
    ui->stackedWidget->setCurrentIndex(2);
}

void Widget::on_pushButton_26_clicked()//按键 ：
{
    DateTimeSetup.append(":");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_31_clicked()//按键 -
{
    DateTimeSetup.append("-");
    ui->lineEdit->setText(DateTimeSetup);
}

void Widget::on_pushButton_30_clicked()//按键 空格
{
    DateTimeSetup.append(" ");
    ui->lineEdit->setText(DateTimeSetup);
}



/****************************************************************/
//滚筒转速 按钮 设置值

void Widget::on_pushButton_12_clicked() //轴流转速 --
{
    if(startflag==0)
     {

         flag_bjd = false;
         tuoliguntong_zs--;
         ui->label_45->setText(QString::number(tuoliguntong_zs,10));
         kaku_tlgt = tuoliguntong_zs;
         startflag = 1;
     }
     else
     {
         flag_bjd = false;
         if(flagjiajia)//承上启下作用标志
         {
              ka_tlgt--;
              kaku_tlgt = ka_tlgt;
             flagjiajia = false;
         }
         else
         {
             kaku_tlgt--;
         }


         ui->label_45->setText(QString::number(kaku_tlgt,10));

         //qDebug()<<"flag_tlgt--"<<kaku_tlgt<<endl;
     }
}

void Widget::on_pushButton_15_clicked()//轴流转速 ++
{
    if(startflag==0)
    {

        flag_bjd = false;
        tuoliguntong_zs++;
        ui->label_45->setText(QString::number(tuoliguntong_zs,10));
        kaku_tlgt = tuoliguntong_zs;
        startflag = 1;
    }
    else
    {
        flag_bjd = false;

        if(flagjiajia)//承上启下作用标志
        {
             ka_tlgt++;
             kaku_tlgt = ka_tlgt;
            flagjiajia = false;
        }
        else
        {
            kaku_tlgt++;
        }

        ui->label_45->setText(QString::number(kaku_tlgt,10));



        //qDebug()<<"flag_tlgt"<<kaku_tlgt<<endl;
    }
}

void Widget::on_pushButton_16_clicked()//复脱转速 --
{
    if(startflag==0)
    {

        flag_bjd = false;
        ftq_zs--;

        ui->label_43->setText(QString::number(ftq_zs,10));
        kaku_ftq = ftq_zs;
        startflag = 1;
    }
    else
    {
        flag_bjd = false;
        if(flagjiajia)//承上启下作用标志
        {
             ka_ftq--;
             kaku_ftq = ka_ftq;
            flagjiajia = false;
        }
        else
        {
            kaku_ftq--;
        }
        ui->label_43->setText(QString::number(kaku_ftq,10));

        //qDebug()<<"flag_123"<<kaku_syq<<endl;
    }
}

void Widget::on_pushButton_17_clicked()//复托转速 ++
{
    if(startflag==0)
    {

        flag_bjd = false;
        ftq_zs++;
         ui->label_43->setText(QString::number(ftq_zs,10));
        kaku_ftq = ftq_zs;
        startflag = 1;
    }
    else
    {
        flag_bjd = false;
        if(flagjiajia)//承上启下作用标志
        {
             ka_ftq++;
             kaku_ftq = ka_ftq;
            flagjiajia = false;
        }
        else
        {
            kaku_ftq++;
        }
        ui->label_43->setText(QString::number(kaku_ftq,10));

        //qDebug()<<"flag_123"<<kaku_syq<<endl;
    }
}

void Widget::on_pushButton_13_clicked()//升运转速 --
{
    if(startflag==0)
    {

        flag_bjd = false;
        syq_zs--;
        ui->label_44->setText(QString::number(syq_zs,10));
        kaku_syq = syq_zs;
        startflag = 1;
    }
    else
    {
        flag_bjd = false;
        if(flagjiajia)//承上启下作用标志
        {
             ka_syq--;
             kaku_syq = ka_syq;
            flagjiajia = false;
        }
        else
        {
            kaku_syq--;
        }


        ui->label_44->setText(QString::number(kaku_syq,10));

        //qDebug()<<"flag_tlgt--"<<kaku_tlgt<<endl;
    }
}

void Widget::on_pushButton_14_clicked()//升运转速 ++
{
    if(startflag==0)
    {

        flag_bjd = false;
        syq_zs++;
        ui->label_44->setText(QString::number(syq_zs,10));
        kaku_syq = syq_zs;
        startflag = 1;
    }
    else
    {
        flag_bjd = false;

        if(flagjiajia)//承上启下作用标志
        {
             ka_syq++;
             kaku_syq = ka_syq;
            flagjiajia = false;
        }
        else
        {
            kaku_syq++;
        }

        ui->label_44->setText(QString::number(kaku_syq,10));



        //qDebug()<<"flag_tlgt"<<kaku_tlgt<<endl;
    }
}
/****************************************************************/



/******************************************************************************************************/
//信号槽
//
//故障码
uchar i = 0;
void Widget::gzmslottest()//故障码显示
{
    if((ecutest.spn_can.if_data == 1)&&(i < ecutest.spn_can.cnt))//是否有数据 0和1  1表示有数据
    {
            if(flagwidget == xingZouWidget)
            {
                ui->label_38->setText(QString::number(ecutest.spn_can.fmi[i]));//FMI
                ui->label_36->setText(QString::number(ecutest.spn_can.spn[i]));//SPN
            }
            else if(flagwidget == workWidget)
            {
                ui->label_5->setText(QString::number(ecutest.spn_can.fmi[i]));//FMI
                ui->label_6->setText(QString::number(ecutest.spn_can.spn[i]));//SPN
            }

//            qDebug()<<"spn_can.fim[i]"<<ecutest.spn_can.fmi[i]<<endl;
//            qDebug()<<"spn_can.spn[i]"<<ecutest.spn_can.spn[i]<<endl;
            i++;
    }
    else
    {
       // qDebug()<<"else else else  ..."<<endl;


//        qDebug()<<"ecutest.spn_can.cnt == "<<ecutest.spn_can.cnt<<endl;
//        qDebug()<<"ecutest.spn_can.if_data == "<<ecutest.spn_can.if_data<<endl;


        i = 0;

        if(flagwidget == xingZouWidget)
        {
            ui->label_38->setText(QString::number(ecutest.spn_can.fmi[i]));//FMI
            ui->label_36->setText(QString::number(ecutest.spn_can.spn[i]));//SPN
        }
        else if(flagwidget == workWidget)
        {
            ui->label_5->setText(QString::number(ecutest.spn_can.fmi[i]));//FMI
            ui->label_6->setText(QString::number(ecutest.spn_can.spn[i]));//SPN
        }

    }


#if 0

    //显示  汉字 报警

            mybufflag[0] = shanshuoSW;//水温报警
            mybufflag[1] = shanshuoJYYL;//机油  油压报警 0～1 MPa,在0.1 MPa 以下为报警区。

            mybufflag[2] = shanshuoYL;//及时加油 油量低报警
            mybufflag[3] = shanshuoYYYW;////液压油温

            mybufflag[4] =  shanshuoLM; //粮箱满 报警

            if((ecutest.flagECU == 1)||(cantest.flagTXCS == 1))
            {
                shanshuoTXGZ = 1;//通信故障
            }
            else
            {
                shanshuoTXGZ = 0;
            }

            mybufflag[5] =  shanshuoTXGZ;  //通信故障


            //建立索引 对mybufflag进行提取。
            for (mm = 0; mm < 15; mm++)
            {
                if(mybufflag[mm])
                {
                    myindex[jflag] = mm;
                    jflag++;

                }

            }

            if(jflag == 0)
            {
                memset(myindex,0,15);
            }


            jjjflag = jflag;
            jflag = 0;

            if (j >= jjjflag)
            {
                j = 0;
                memset(myindex,0,15);
            }
    /**********************************************************************************************************/
#endif


//qDebug()<<"jjjflag"<<jjjflag<<endl;
//qDebug()<<"j = "<<j<<endl;

if(j<jjjflag)
{
    switch(myindex[j])
    {
        case 0:
        //ui->label_11->setText(QObject::tr("水温偏高"));
        if(flagwidget == xingZouWidget)
        {
            ui->label_29->setPixmap(QPixmap("./img2/iconback/41.png"));
        }
        else if(flagwidget == workWidget)
        {
            ui->label_11->setPixmap(QPixmap("./img2/iconback/41.png"));
        }
        break;

        case 1:
        //ui->label_11->setText(QObject::tr("油压偏低"));

        if(flagwidget == xingZouWidget)
        {
            ui->label_29->setPixmap(QPixmap("./img2/iconback/42.png"));
        }
        else if(flagwidget == workWidget)
        {
            ui->label_11->setPixmap(QPixmap("./img2/iconback/42.png"));
        }
        break;

        case 2:

//        ui->label_11->setStyleSheet("color:yellow;");
//        ui->label_11->setText(QObject::tr("请及时加油"));

        if(flagwidget == xingZouWidget)
        {
            ui->label_29->setPixmap(QPixmap("./img2/iconback/47.png"));
        }
        else if(flagwidget == workWidget)
        {
            ui->label_11->setPixmap(QPixmap("./img2/iconback/47.png"));
        }
        break;

        case 3:
        //ui->label_11->setText(QObject::tr("液压油温偏高请检查"));

        if(flagwidget == xingZouWidget)
        {
            ui->label_29->setPixmap(QPixmap("./img2/iconback/46.png"));
        }
        else if(flagwidget == workWidget)
        {
            ui->label_11->setPixmap(QPixmap("./img2/iconback/46.png"));
        }

        break;
        case 4:
        //ui->label_11->setText(QObject::tr("粮箱已满"));

        if(flagwidget == xingZouWidget)
        {
            ui->label_29->setPixmap(QPixmap("./img2/iconback/45.png"));
        }
        else if(flagwidget == workWidget)
        {
            ui->label_11->setPixmap(QPixmap("./img2/iconback/45.png"));
        }

        break;

        case 5:
        //ui->label_11->setText(QObject::tr("通信故障"));

        if(flagwidget == xingZouWidget)
        {
            ui->label_29->setPixmap(QPixmap("./img2/iconback/48.png"));
        }
        else if(flagwidget == workWidget)
        {
            ui->label_11->setPixmap(QPixmap("./img2/iconback/48.png"));
        }

        break;

        case 6:
         //ui->label_11->setPixmap(QPixmap("./img2/iconback/49.png"));//手刹

         if(flagwidget == xingZouWidget)
         {
             ui->label_29->setPixmap(QPixmap("./img2/iconback/49.png"));
         }
         else if(flagwidget == workWidget)
         {
             ui->label_11->setPixmap(QPixmap("./img2/iconback/49.png"));
         }

         break;

        default:
        // qDebug()<<"default default default"<<endl;
        break;

    }
    j++;
}
else
{
     j = 0;
     //ui->label_11->setText(QObject::tr("一二三四五六七八九"));//一二三四五六七八九
     //qDebug()<<"WWWWWWWWWWWWW++++++++++++++++++++++++++++++++++++++++++++++++++99999999999999999999999"<<endl;
     //ui->label_11->setText(QObject::tr("  "));//一二三四五六七八九一二三四五六七八九


     if(flagwidget == xingZouWidget)
     {
         ui->label_29->setText(QObject::tr("  "));//一二三四五六七八九一二三四五六七八九
     }
     else if(flagwidget == workWidget)
     {
         ui->label_11->setText(QObject::tr("  "));//一二三四五六七八九一二三四五六七八九
     }
}

}

//平滑转动
void Widget::shanhua()//闪烁和平滑转动
{
#if 0
    if(flagadd)
    {
        numtmp++;
        if(numtmp>132)
        {
            flagadd = 0;
        }
    }
    else
    {
        numtmp--;
        if(numtmp==0)
        {
            flagadd = 1;
        }
    }

#endif

    nu2 = (ecutest.FDJ_speed/100);
    //nu2 = ecutest.FDJ_speed;
    if(nu2 > 30)
    {
        nu2 = 30;
    }

    if(nu1 < nu2)//cantest.FDJ_speed
    {
        nu1++;
        //qDebug()<<"++"<<cantest.FDJ_speed<<endl;
    }
    else if(nu1 > nu2)//cantest.FDJ_speed
    {
        nu1--;
        //qDebug()<<"------"<<cantest.FDJ_speed<<endl;
    }


    floatnu1 = ecutest.FDJ_speed;
    floatnu1 = floatnu1/100;
    if(floatnu1 >30)
    {
        floatnu1 = 30;
    }


    nu4 = (cantest.HourSpeed/10);///100

    floatnu3 = cantest.HourSpeed;
    floatnu3 /= 10;

    //qDebug()<<"cantest.HourSpeed------ =  "<<cantest.HourSpeed<<endl;

    if(nu4>30)
    {
        nu4 = 30;
    }
    if(nu4<0)
    {
        nu4 = 0;
    }
    if(nu3 < nu4)//cantest.FDJ_speed
    {
        nu3++;
        //qDebug()<<"3++"<<cantest.FDJ_speed<<endl;
    }
    else if(nu3 > nu4)//cantest.FDJ_speed
    {
        nu3--;

        //qDebug()<<"4------"<<cantest.FDJ_speed<<endl;
    }

    if(flagaddnum)
    {

        //闪烁控制

        if(ecutest.flagECU == 0)
        {
            if(ecutest.VolSW > 102)//水温
            {
                shanshuoSW = 1;
            }

            if(JYYL < 0.08)//机油
            {
                shanshuoJYYL = 1;
            }
        }


        if(cantest.flagTXCS == 0)//cantest.flagTXCS == 1 mcu  故障
        {
            if(cantest.VolYL <= 1)//油量
            {
                shanshuoYL = 1;
            }
        }

        //油中有水
        if(ecutest.flagYSFL_ECU == 1)//油中有水
        {
            shanshuoYZYS = 1;
        }

        //液压油温
        if(cantest.VolYeyayouwen > 90)
        {
            shanshuoYYYW = 1;
        }

        //手刹
        if(cantest.flagSS)
        {
            shanshuoSS = 1;
        }
        //粮满
        if(cantest.flagLCM)
        {
            shanshuoLM = 1;
        }


        if(flagDelay)//第一次上电
        {

            if(cantest.ZLspeed<ruby[0])//轴流滚筒转速
            {
                //if (cantest.ZLspeed != 0)
                {
                    shanshuozlzs = 1;//轴流滚筒转速
                }

            }

            if(cantest.FTspeed<ruby[1])//复脱器转速
            {
                //if (cantest.FTspeed != 0)
                {
                    shanshuoftqzs = 1;//复托器转速
                }

            }

            if(cantest.SYspeed<ruby[2])//升运器堵塞转速
            {
                //if (cantest.SYspeed != 0)
                {
                     shanshuosyqzs = 1;//升运器转速
                }

            }

        }





        //显示  汉字 报警

                mybufflag[0] = shanshuoSW;//水温报警
                mybufflag[1] = shanshuoJYYL;//机油  油压报警 0～1 MPa,在0.1 MPa 以下为报警区。

                mybufflag[2] = shanshuoYL;//及时加油 油量低报警
                mybufflag[3] = shanshuoYYYW;////液压油温

                mybufflag[4] =  shanshuoLM; //粮箱满 报警

                if((ecutest.flagECU == 1)||(cantest.flagTXCS == 1))
                {
                    shanshuoTXGZ = 1;//通信故障
                }
                else
                {
                    shanshuoTXGZ = 0;
                }

                mybufflag[5] =  shanshuoTXGZ;  //通信故障

                mybufflag[6] =  shanshuoSS;  //手刹


                //建立索引 对mybufflag进行提取。
                for (mm = 0; mm < 15; mm++)
                {
                    if(mybufflag[mm])
                    {
                        myindex[jflag] = mm;
                        jflag++;

                    }

                }

                if(jflag == 0)
                {
                    memset(myindex,0,15);
                }


                jjjflag = jflag;
                jflag = 0;

                if (j >= jjjflag)
                {
                    j = 0;
                    memset(myindex,0,15);
                }
        /**********************************************************************************************************/





        //闪烁算法
        flagnum++;
        if(flagnum>15)//30
        {
            flagaddnum = 0;
        }
    }
    else
    {
        shanshuoSW = 0;//水温
        shanshuoJYYL = 0;//机油
        shanshuoYL = 0;//油量
        shanshuoYZYS = 0;// 油中有水

        shanshuoSS = 0;//手刹
        shanshuoLM = 0;//粮满

        shanshuozlzs = 0;//轴流滚筒转速
        shanshuoftqzs = 0;//复托器转速
        shanshuosyqzs = 0;//升运器转速

        flagnum--;
        if(flagnum==0)
        {
            flagaddnum = 1;
        }
    }
}

//里程 槽函数
void Widget::Licheng()//里程
{
#if 1
       // worktimer_Licheng->stop();


        //if((ecutest.flagECU == 0)&&(ecutest.FDJ_speed>350))
        if((ecutest.flagECU == 0))
        {
            if(((shanshuoYZYS == 1)||(shanshuoSW == 1)||(shanshuoJYYL ==1)||(shanshuoLM == 1)||(shanshuoSS == 1)||(shanshuozlzs == 1)||(shanshuoftqzs == 1)||(shanshuosyqzs == 1))&&(ecutest.FDJ_speed>350))//||shanshuoYL||shanshuozlzs||shanshuoftqzs||shanshuosyqzs
            {
                if(flagbeep)
                {
                    beep_on();
                    //qDebug()<<"beef on 00000000000000000000000000000000000000000000000000000000   ======   "<<ecutest.flagECU<<endl;

                }
               // qDebug()<<"beef on 00000000000000000000000000000000000000000000000000000000   ======   "<<ecutest.flagECU<<endl;
//                qDebug()<<"beef on shanshuoJYYL  ==                  ff         == "<<shanshuoJYYL<<endl;

            }
            else if((shanshuoYZYS != 1)||(shanshuoSW != 1)||(shanshuoJYYL !=1)||(shanshuoLM != 1)||(shanshuoSS != 1)||(shanshuozlzs != 1)||(shanshuoftqzs != 1)||(shanshuosyqzs != 1))
            {
                if(flagbeepzero == 0)
                {
                    beep_off();
                    //qDebug()<<"beef off 999999999999999999999999999999999999999999999999999999999"<<endl;

                }
               // qDebug()<<"beef off 999999999999999999999999999999999999999999999999999999999"<<endl;
//                //qDebug()<<"shanshuoYL  == "<<shanshuoYL<<endl;
            }
        }
        else if (ecutest.flagECU == 1)//||(ecutest.FDJ_speed < 350))
        {
            beep_off();
        }
#endif

////////////////////////////////////////////////////////////////////////////////////
//发动机转速达到 350时 蜂鸣器报警 。
#if 1
        if(cantest.flagTXCS != 1)
        {
                if((shanshuoYL == 1))
                {
                    DelayYLcounter++;
                    if(DelayYLcounter == 60)
                    {
                        if(flagbeep)
                        {
                            beep_on();
                            flagbeepzero = 1;
                        }
                        //qDebug()<<"DelayYLcounter  988888888888888888888888888888888888888888888888888== "<<DelayYLcounter<<endl;
                        DelayYLcounter = 0;
                    }
                }
                else if((shanshuoYL != 1)&&(flagbeepzero == 1))
                {
                    Delagbeepoff++;
                    if(Delagbeepoff == 1)
                    {
                    beep_off();
                    flagbeepzero = 0;

                    }
                    //qDebug()<<"offfffffffffffff  == "<<DelayYLcounter<<endl;
                     Delagbeepoff = 0;
                }
        }


#endif
}

//小时计 槽函数
void Widget::xiaoshiji()//小时计
{
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());//汉字显示
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      db = QSqlDatabase::database("qt_sql_default_connection");
    else
      db = QSqlDatabase::addDatabase("QSQLITE");


   //QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName("jy.db");
   if (!db.open())
   {
       qDebug()<<"open database failed ---"<<db.lastError().text()<<"/n";
   }
   QSqlQuery query;
   #if 0
   bool ok = query.exec("create table xsj(id INTEGER,name varchar,age INTEGER)");
   if (ok)
   {
       qDebug()<<"ceate table partition success"<<endl;
   }
   else
   {
       qDebug()<<"ceate table partition failed"<<endl;
   }
   #endif

   query.exec("select id, name, age from xsj");
   while (query.next())
   {

     // qDebug()<<"id("<<query.value(0).toInt()<<")  name:"<<query.value(1).toString()<<"  age:"<<query.value(2).toInt();
     //xiaoshiJi_m = query.value(2).toInt();

       xiaoshiJi_m = query.value(2).toLongLong();

   }

  if(ecutest.FDJ_speed>350)
   {
        xiaoshiJi_m++;
   }
    xiaoshiJi_h = (xiaoshiJi_m/3600)*1000 + ((xiaoshiJi_m%3600)*1000)/3600;
    xiaoshiJi_h /= 1000;

    if(flagwidget == xingZouWidget)
    {
        ui->label_33->setText(QString::number(xiaoshiJi_h,'f',1));
    }
    else if(flagwidget == workWidget)
    {
        ui->label_7->setText(QString::number(xiaoshiJi_h,'f',1));
    }
          //query.prepare("INSERT INTO xsj (id, name, age) VALUES (:id, :name, :age)");

          query.prepare("update xsj set age = :age where id = :id");

          //query.prepare(update_sql);
          query.bindValue(":id",1);
         // query.bindValue(":name", QObject::tr("小时计"));
          query.bindValue(":age", xiaoshiJi_m);
          query.exec();

        query.exec("select id, name, age from xsj");
        while (query.next())
        {

           //qDebug()<<"id("<<query.value(0).toInt()<<")  name:"<<query.value(1).toString()<<"  age:"<<query.value(2).toInt();
        }

        query.exec(QObject::tr("drop xsj"));

/*******************************************************************/

        if((cantest.ZLspeed != 0)||(cantest.FTspeed != 0) ||(cantest.SYspeed != 0))
        {
            DelayCount++;
            if(DelayCount == 7)
            {
                flagDelay = 1;
            }
        }
        else if((cantest.ZLspeed == 0)&&(cantest.FTspeed == 0)&&(cantest.SYspeed == 0))
        {
            flagDelay = 0;
            DelayCount = 0;
           // qDebug()<<"delay ........                        ...................."<<endl;

        }
}
