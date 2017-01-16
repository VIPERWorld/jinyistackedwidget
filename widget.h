#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
private:
    Ui::Widget *ui;

    QTimer *timer_Main;
    QTimer *timer_xiaoshiji;
    QTimer *timer_shanhua;
    QTimer *timergzm;
    QTimer *timer_Licheng;//里程

protected:
    void paintEvent(QPaintEvent *);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();

    void on_pushButton_32_clicked();
    void on_pushButton_22_clicked();
    void on_pushButton_19_clicked();
    void on_pushButton_33_clicked();
    void on_pushButton_29_clicked();
    void on_pushButton_28_clicked();
    void on_pushButton_27_clicked();
    void on_pushButton_18_clicked();
    void on_pushButton_23_clicked();
    void on_pushButton_21_clicked();
    void on_pushButton_24_clicked();
    void on_pushButton_25_clicked();
    void on_pushButton_20_clicked();
    void on_pushButton_26_clicked();
    void on_pushButton_31_clicked();
    void on_pushButton_30_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_17_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();



    void xiaoshiji();//小时计
    void Licheng();//里程
    void shanhua();//闪烁和平滑转动
    void gzmslottest();//故障码
};

#endif // WIDGET_H
