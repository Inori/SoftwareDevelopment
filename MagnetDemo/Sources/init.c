#include "include.h"

 


void SetBusCLK_80M(void)//将单片机主频设为80MHz
{   
  CLKSEL = 0x00;				//disengage PLL to system
  PLLCTL_PLLON = 1;
  SYNR = 0b11001001;    //VCOFRQ=11, SYNDIV=0b001001=9                    
  REFDV = 0b10000001;   //REFFRQ=10, REFDIV=0b000001=1
                        //核心板上晶振fOSC=16M 
                        //fVCO=2*fOSC*(SYNDIV+1)/(REFDIV+1)=2*16M*10/2=160M
  POSTDIV = 0x00;       //POSTDIV=0, fPLL=fVCO=160M
                        //fBUS=fPLL/2=80M
                        //fREF=fOSC/(REFDIV+1)=8M  
  _asm(nop);
  _asm(nop);
  while(!(CRGFLG_LOCK == 1));	  //when pll is steady ,then use it;
  CLKSEL_PLLSEL = 1;		        //engage PLL to system;
}

void initRTI(void)    //Real Time Interrupt，实时中断，每10ms中断一次
{
  RTICTL= 0b11000111;    //十进制，16M/(16*10*10^3)=100;
  CRGINT|=0b10000000;   //使能RTI    
}


void initPIT(void)      //周期中断初始化
{   
   PITCFLMT_PITE=0;     //disable PIT
   
   PITCE_PCE0=1;        //enable timer channel 0
   PITMUX_PMUX0=0;      //ch0 connected to micro timer 0
   //设置微定时器和定时器周期
   PITMTLD0=250-1;      
   PITLD0=3200-1;      //250 * 3200 / 80000000 = 10ms. --> 仅为实验值，有待后期调整
   
   PITINTE_PINTE0=1;    //enable interupt channel 0
   
   PITCFLMT_PITE=1;     //enable PIT
}

void initPWM(void)     //PWM初始化
{
  PWME = 0x00;         //禁止PWM
  PWMPRCLK = 0x00;     //clockA 1分频:80MHz   clockB 1分频:80MHz
  
  //*****转向舵机 6、7级联********
  PWMCTL_CON67 = 1;    //67级联
  PWMSCLB = 40;        //对clock SB 进行2*PWMSCLB=80分频；pwm clock=clockB/80=1MHz;  
  PWMPOL_PPOL7 = 1;    //信号以高电平开始
  PWMCLK_PCLK7 = 1;    //时钟为SB
  PWMCAE_CAE7 = 0;     //左对齐
  PWMPER67 = 20000;    //方波上限值为20000，PWM频率 = SB/20000 = 50Hz
  PWMDTY67 = MID;     //占空比为转向舵机中值位置
  PWME_PWME7 = 1;      //使能PWM67
  //******************************
  
  //*****电机脉冲1 4、5级联*******
  PWMCTL_CON45=1;      //45级联
  PWMPOL_PPOL5 = 1;    //信号以高电平开始
  PWMCLK_PCLK5 = 0;    //时钟为A
  PWMCAE_CAE5 = 0;     //左对齐
  PWMPER45 = 5000;     //PWM频率 = clockA/5000 = 16kHz
  PWMDTY45 = 0;        //占空比0，电机不工作
  PWME_PWME5 = 1;      //使能PWM45
  //******************************
  
  //*****电机脉冲2 2、3级联*******
  PWMCTL_CON23=1;      //23级联
  PWMPOL_PPOL3 = 1;    //信号以高电平开始
  PWMCLK_PCLK3 = 0;    //时钟为B
  PWMCAE_CAE3 = 0;     //左对齐
  PWMPER23 = 5000;     //PWM频率 = clockB/5000 = 16kHz
  PWMDTY23 = 0;        //占空比0，电机不工作
  PWME_PWME3 = 1;      //使能PWM23
  //******************************
}

void initPORT(void)     //IO口初始化
{
  DDRB=0xFF;            //B口置为输出，接屏幕
  DDRS=0x00;            //S口置为输入，接旋钮1
  PORTB=0xFF;
  DDRE=0x00;            //E口置为输入，接旋钮2、3
}

void initTIM(void)     //输入捕捉中断初始化、脉冲累加器初始化
{
 
  TIOS =0x00;          //定时器通道为输入捕捉
  TSCR1=0x80;          //定时器使能
  TCTL4=0b00010101;    //T0捕捉旋钮3上升沿, T1捕捉旋钮1上升沿, T2捕捉旋钮2上升沿
  TFLG1=0xFF;          //清中断标志位
  TIE=0x00;            
}

void initPAC(void)
{
  PACTL=0x40;          //使能PT7脉冲累加器
  PACNT=0;        //将计数器清零
}


void initATD(void)
{
  ATD0CTL0=0x02;       //转换完AD2后回到AD0，即按照AD0、AD1、AD2、AD0、AD1......顺序转换
  ATD0CTL1=0x30;       //不用外部触发AD转换，转换精度10位，采样前放电（放电测得的转换值更精确，但需要额外的时钟周期，到底需不需要放电请各位自行调试）
  ATD0CTL2=0x40;       //快速清零 无等待模式, 中断禁止
  ATD0CTL3=0x98;       //ATD转换结果右对齐，转换序列长度是3，即转换AD0、AD1、AD2三个，禁用FIFO模式
  ATD0CTL4=0x01;       //4周期,ATDCLK=fBUS/(2*(PRS+1))=80M/(2*（1+1))=20M
  ATD0CTL5=0x30;       //连续多通道转换
}

void Init_All(void)

{
  SetBusCLK_80M();
  initPORT();
  initLCD();
//  initRTI();  
  initPIT();
  initPWM();
//  initTIM();
  initPAC();
  initATD();
}