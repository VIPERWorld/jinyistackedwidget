#ifndef _LIB_SHM_H_
#define _LIB_SHM_H_

#ifdef __cplusplus
extern "C"{
#endif

#define	u8 unsigned char
#define	u16 unsigned short
#define	u32 unsigned int
#define RAM_CAN_fData                   "RAM_CAN_fData"         // CAN共享内存文件
#define RAM_CAN_ECU_fData               "RAM_CAN_ECU_fData"         // CANECU共享内存文件

#define CAN_LOCK                        1024+1               // CAN共享锁
/************************************************/
/* CAN 中间层数据传输结构 */
/************************************************/

typedef struct
{
	int cnt;//接收SPN包书
	int if_data;//是否有数据
	int spn[10];
	int fmi[10];
}CAN_SPN;

typedef struct
{
	unsigned char  can_no;
	u8 flagTXCS;//通信超时标志
	//指示标志:0,1
	//开关量 输入 
	u8 flagLeft;  //左转
	u8 flagBattery;//电瓶指示灯 充电指示灯
	u8 flagWidthlamp; //示宽灯
	u8 flagYG; //远光灯
	//u8 flagSW; //水温
	u8 flagJG; //近光灯
	u8 flagJY; //机油
	u8 flagLCM; //粮仓满
	u8 flagLCM_seven; //粮仓满70
	u8 flagGL; //过滤
	u8 flagKL; //空气滤
	u8 flagSS;//手刹
	u8 flagFDJYR; //发动机预热
	u8 flagPark;//停车
	u8 flagFDJGZ; //发动机故障
	u8 flagRight; //右转
	u8 flagDC;//倒车信号

	u8 flagYYMainSK;//液压主离合松开
	u8 flagYYMainZJ;//液压主离合张紧
	u8 flagBHLdown;//拔禾轮降低
	u8 flagBHLup;//拔河轮升
	u8 flagGTup;//割台升
	u8 flagGTdown;//割台降
	u8 flagfastspeed;//高速信号
	u8 flagXLTclose;//卸粮筒关闭
	u8 flagXLTopen;//卸粮筒打开
	u8 flagYSFL;//油水分离
	//u8 flagCD;//充电指示

	//开关量输出
	u8 flagBHLdownDCF;//拔禾轮降低电磁阀
	u8 flagBHLupDCF;//拔河轮升电磁阀
	u8 flagYYMainSKDCF;//液压主离合松开电磁阀
	u8 flagYYMainZJDCF;//液压主离合张紧电磁阀
	u8 flagGTupDCF;//割台升
	u8 flagGTdownDCF;//割台降
	u8 flagXLTcloseDCF;//卸粮筒关闭
	u8 flagXLTopenDCF;//卸粮筒打开
	u8 flagSCDCF;//刹车电磁阀
	u8 flagDCPower;//倒车电源
	u8 flagYLDCF;//溢流电磁阀
	u8 flagmijiclr;//米计清零
	u8 flagKD;//空挡标示
	u16 speed;
	//指示数值
	int VolYL; //油量
	int VolYeyayouwen;//液压油温
	int MIJI;//米计
	int SaLiangLV;//撒粮损失率
	int GeChaheight;//割茬高度
	int FTspeed;//复脱转速
	int SYspeed;//升运转速
	int ZLspeed;//轴流转速
	int HourSpeed;//时速
	int VOLV;//电池电压
	int VolMIJISUM;//总米计
	int VolLICHENG;//里程
	int VolMCL;//亩产量
	int VolGTweith;//割台宽度
	int VolShiWen;//室温
} RAM_CAN_sData;

typedef struct
{
	unsigned char  can_no;
	u8 flagFDJYR_ECU; //发动机预热ECU
	u8 flagECU;//ecu
	u8 flagFDJGZ_ECU; //发动机故障ECU
	u8 flagYSFL_ECU;//油水分离ECU

	//指示数值
	int VolSW;//水温
	int VolYL; //油量
	int VolJYWD;//机油温度
	int VolJYYL;//机油压力
	int FDJ_speed;//发动机转速

	//SPN故障
	CAN_SPN spn_can;
} RAM_CAN_sData_ECU;



void Can_Ram_init();
void Can_Ram_Get(RAM_CAN_sData * RAM_CAN,RAM_CAN_sData_ECU * RAM_CAN_ECU);
void Can_Ram_Set(RAM_CAN_sData * RAM_CAN,RAM_CAN_sData_ECU * RAM_CAN_ECU);
void Can_set_spn_flag(int flag);
void Can_set_mijiclr_flag(int flag);
void Can_set_MCL_flag(int chanliang);
void Can_set_GTweith_flag(int weith);

//加共享内存锁
void lockshm();
//解共享内存锁
void unlockshm();
void Set_IO_H(int gpion);
void Set_IO_L(int gpion);
//int beep_Init();
//打开蜂鸣器
void beep_on();

//关闭蜂鸣器
void beep_off();
//打开蜂鸣器,短响
void beep_on_Bit();

//摄像头切换到V2
void cam_to_V2();
//摄像头切换到V3
void cam_to_V3();
//打开蜂鸣器,长闪
//void beep_on_S_short();

//打开蜂鸣器,短闪
//void beep_on_S_long();

#ifdef __cplusplus
}
#endif 

#endif
