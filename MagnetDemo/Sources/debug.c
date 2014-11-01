#include "include.h"

extern CENTER;
extern LEFT;
extern RIGHT;
extern SPEED;

extern T ;

extern status; 
                           
extern status_knob;   
                                                    
extern last_press_time;  
extern last_turn_time;   
extern value;    


unsigned char status_button=0;  //旋钮按下状态变量
char mystr[20]="";              //屏幕显示字符缓存数组
char temp_status=1;             //调试阶段某个过程中的状态标记变量

//电机变量,暂时先放到本文件内，调试过后需转移到算法控制的c文件中

int pre_error = 0;                //上一次采样的速度偏差
int d_pre_error = 0;              //上一次采样的速度偏差的微分
unsigned short int maxspeed = 1000;//最高限速（脉冲沿个数）
unsigned short int minspeed = 0;   //最低限速（脉冲沿个数）
unsigned short int target = 0;    //目标速度
char Kp = 10;                     //PID车速控制的比例系数
char Ki = 2;                      //PID车速控制的积分系数
char Kd = 2;                      //PID车速控制的微分系数
int error = 0;              //实际速度与目标速度的偏差
int d_error = 0;            //速度偏差的微分
int dd_error = 0;           //速度偏差的二阶微分
int result_motor=0;         //PID的计算结果

#define steering PWMDTY67


unsigned char check_button(void)  //检测旋钮按键是否按下的函数，旋钮1按下则返回1，旋钮2按下则返回2，旋钮3按下则返回3，否则返回0
                                  //本函数无须过度关注
{
  if(T-last_press_time>30)
  {
    if(!PTS_PTS3)
    {
      last_press_time=T;
      while(!PTS_PTS3)             //若旋钮1被按下
      {
        if(T - last_press_time > 10)//维持按下状态的时间超过10/100=100ms，则认为旋钮1被按下
        {
          LCD_CLS();
          return 1;                 //返回1
        }
      }
    }
    else if(!PORTE_PE3)
    {
      last_press_time=T;
      while(!PORTE_PE3)               //若旋钮2被按下
      {
        if(T - last_press_time > 10)  //维持按下状态的时间超过10/100=100ms，则认为旋钮2被按下
        {
          LCD_CLS();
          return 2;                   //返回2
        }
      }
    }
    else if(!PORTE_PE5)
    {
      last_press_time=T;
      while(!PORTE_PE5)               //若旋钮3被按下
      {
        if(T - last_press_time > 10)  //维持按下状态的时间超过10/100=100ms，则认为旋钮3被按下
        {
          LCD_CLS();
          return 3;                   //返回3
        }
      }
    }
  }
  return 0;                       //若旋钮1、2均没有被持续按下超过100ms，则认为没有旋钮被按下，返回0
}

void wait(unsigned long time)  //延时函数，参数为延时时间，单位是1/100 s
{
  unsigned long temptime = T;
  while(T - temptime < time);
}

void Debug(void)

{
  LCD_P6x8Str(0,0,"TJU Intelligent Car"); //屏幕第1、3、5、7行显示TJU Intelligent Car
  LCD_P6x8Str(0,2,"TJU Intelligent Car");
  LCD_P6x8Str(0,4,"TJU Intelligent Car");
  LCD_P6x8Str(0,6,"TJU Intelligent Car");
  
  wait(100);         //延时1s
  
  LCD_CLS();         //清空屏幕

  LCD_P6x8Str(0,0,"Left=");
  LCD_P6x8Str(0,1,"Center=");
  LCD_P6x8Str(0,2,"Right=");
  
  for(;;)  //开机后进入调试阶段，这里是调试阶段的死循环，会在适当时候break掉，之后小车将启动
  {
    //启动前小车的调试状态status，0为屏幕显示3个电感采集到的信号，经过AD转换后的数值
    //1为调整舵机中值，2为调整PID参数，3为设定运行速度，4启动
    
    if(status==0)       //屏幕显示电感信号值
    {
      sprintf(mystr,"%04d",LEFT);
      LCD_P6x8Str(30,0,mystr);
      sprintf(mystr,"%04d",CENTER);
      LCD_P6x8Str(42,1,mystr);
      sprintf(mystr,"%04d",RIGHT);
      LCD_P6x8Str(36,2,mystr);
      
      status_button=check_button();
      if(status_button==1)      //按下旋钮1进入参数调整阶段，先进入调整舵机阶段
      {
        value=steering;
        LCD_P6x8Str(0,7,"STEER=");
        status++;
        TFLG1=0xFF;
        TIE = 0b111;          //使能旋转编码器中断，禁止行场中断
      }
      else if(status_button==2)//按下旋钮2进入速度设定阶段，准备启动
      {
        LCD_P6x8Str(0,6,"MAX=");
				LCD_P6x8Str(0,7,"MIN=");
        status=3;
        TFLG1=0xFF;
        TIE = 0b111;          //使能旋转编码器中断，禁止行场中断
      }
    }
    else if(status==1)        //调整舵机中值，旋转旋钮1精调舵机，旋转旋钮2粗调舵机
    {
      switch(status_knob)
      {
        case 1:
        {
          value += 10;
          status_knob=0;
          break;
        }
        case 2:
        {
          value -= 10;
          status_knob=0;
          break;
        }
        case 3:
        {
          value += 100;
          status_knob=0;
          break;
        }
        case 4:
        {
          value -= 100;
          status_knob=0;
          break;
        }
      }
      steering=value;
      
      sprintf(mystr,"%4d",value);
			LCD_P6x8Str(36,7,mystr);
			
      status_button=check_button();//按下旋钮1进入PID调试阶段
      if(status_button==1)
      {
        value=target;
        status++;
        LCD_P6x8Str(0,2,"*Kp=");
				LCD_P6x8Str(0,3," Ki=");
				LCD_P6x8Str(0,4," Kd=");
				LCD_P6x8Str(0,5," target=");
				LCD_P6x8Str(0,6," PWM=");
				LCD_P6x8Str(0,7," speed=");        
      }
      else if(status_button==2)//按下旋钮2回到传感器数值显示阶段
      {
        status=0;
        LCD_P6x8Str(0,0,"Left=");
        LCD_P6x8Str(0,1,"Center=");
        LCD_P6x8Str(0,2,"Right=");        
        TFLG1=0xFF;
        TIE=0x00;
      }
    }
    
    else if(status==2)  //调整PID参数，旋转旋钮1用于调整选定的参数，旋钮2用于选定调整哪个参数
    {
      switch(status_knob)
      {
        case 1:
        {
          switch(temp_status)
          {
            case 1:
            {
              Kp++;
              if(Kp>30)
                Kp=30;
      				break;
            }
            case 2:
            {
              Ki++;
              if(Ki>30)
                Ki=30;
      				break;
            }
            case 3:
            {
              Kd++;
              if(Kd>30)
                Kd=30;
      				break;
            }
            case 4:
            {
              value+=100;
              if(value>1000)
                value=1000;
              target=value;
      				break;
            }
          }
          status_knob=0;
          break;
        }
        case 2:
        {
          switch(temp_status)
          {
            case 1:
            {
              Kp--;
              if(Kp<0)
                Kp=0;
      				break;
            }
            case 2:
            {
              Ki--;
              if(Ki<0)
                Ki=0;
      				break;
            }
            case 3:
            {
              Kd--;
              if(Kd<0)
                Kd=0;
      				break;
            }
            case 4:
            {
              value-=100;
              if(value<0)
                value=0;
              target=value;
      				break;
            }
          }
          status_knob=0;
          break;
        }
        case 3:
        {
          temp_status++;
          if(temp_status>4)
            temp_status=1;
          switch(temp_status)
          {
            case 1:
            {
              LCD_P6x8Str(0,2,"*Kp=");
      				LCD_P6x8Str(0,3," Ki=");
      				LCD_P6x8Str(0,4," Kd=");
      				LCD_P6x8Str(0,5," target=");
      				LCD_P6x8Str(0,6," PWM=");
      				LCD_P6x8Str(0,7," speed=");
      				break;
            }
            case 2:
            {
              LCD_P6x8Str(0,2," Kp=");
      				LCD_P6x8Str(0,3,"*Ki=");
      				LCD_P6x8Str(0,4," Kd=");
      				LCD_P6x8Str(0,5," target=");
      				LCD_P6x8Str(0,6," PWM=");
      				LCD_P6x8Str(0,7," speed=");
      				break;
            }
            case 3:
            {
              LCD_P6x8Str(0,2," Kp=");
      				LCD_P6x8Str(0,3," Ki=");
      				LCD_P6x8Str(0,4,"*Kd=");
      				LCD_P6x8Str(0,5," target=");
      				LCD_P6x8Str(0,6," PWM=");
      				LCD_P6x8Str(0,7," speed=");
      				break;
            }
            case 4:
            {
              LCD_P6x8Str(0,2," Kp=");
      				LCD_P6x8Str(0,3," Ki=");
      				LCD_P6x8Str(0,4," Kd=");
      				LCD_P6x8Str(0,5,"*target=");
      				LCD_P6x8Str(0,6," PWM=");
      				LCD_P6x8Str(0,7," speed=");
      				break;
            }
          }
          status_knob=0;
          break;
        }
        case 4:
        {
          temp_status--;
          if(temp_status<1)
            temp_status=4;
          
          switch(temp_status)
          {
            case 1:
            {
              LCD_P6x8Str(0,2,"*Kp=");
      				LCD_P6x8Str(0,3," Ki=");
      				LCD_P6x8Str(0,4," Kd=");
      				LCD_P6x8Str(0,5," target=");
      				LCD_P6x8Str(0,6," PWM=");
      				LCD_P6x8Str(0,7," speed=");
      				break;
            }
            case 2:
            {
              LCD_P6x8Str(0,2," Kp=");
      				LCD_P6x8Str(0,3,"*Ki=");
      				LCD_P6x8Str(0,4," Kd=");
      				LCD_P6x8Str(0,5," target=");
      				LCD_P6x8Str(0,6," PWM=");
      				LCD_P6x8Str(0,7," speed=");
      				break;
            }
            case 3:
            {
              LCD_P6x8Str(0,2," Kp=");
      				LCD_P6x8Str(0,3," Ki=");
      				LCD_P6x8Str(0,4,"*Kd=");
      				LCD_P6x8Str(0,5," target=");
      				LCD_P6x8Str(0,6," PWM=");
      				LCD_P6x8Str(0,7," speed=");
      				break;
            }
            case 4:
            {
              LCD_P6x8Str(0,2," Kp=");
      				LCD_P6x8Str(0,3," Ki=");
      				LCD_P6x8Str(0,4," Kd=");
      				LCD_P6x8Str(0,5,"*target=");
      				LCD_P6x8Str(0,6," PWM=");
      				LCD_P6x8Str(0,7," speed=");
      				break;
            }
          }
          status_knob=0;
          break;          
        }
      }
      
      sprintf(mystr,"%3d",Kp);
      LCD_P6x8Str(24,2,mystr);
      sprintf(mystr,"%3d",Ki);
      LCD_P6x8Str(24,3,mystr);
      sprintf(mystr,"%3d",Kd);
      LCD_P6x8Str(24,4,mystr);
      sprintf(mystr,"%4d",target);
      LCD_P6x8Str(48,5,mystr);
      sprintf(mystr,"%4d",PWMDTY45);
      LCD_P6x8Str(30,6,mystr);
      sprintf(mystr,"%4d",SPEED);
      LCD_P6x8Str(42,7,mystr);
      
      status_button=check_button();
      if(status_button==1)
      {
        LCD_P6x8Str(0,2,"*Kp=");
				LCD_P6x8Str(0,3," Ki=");
				LCD_P6x8Str(0,4," Kd=");
				LCD_P6x8Str(0,5," target=");
				LCD_P6x8Str(0,6," PWM=");
				LCD_P6x8Str(0,7," speed=");        
      }
      else if(status_button==2)//按下旋钮1无反应，按下旋钮2回到传感器数值显示阶段
      {
        target=0;
        status=0;
        LCD_P6x8Str(0,0,"Left=");
        LCD_P6x8Str(0,1,"Center=");
        LCD_P6x8Str(0,2,"Right=");
        status_knob=0;
        TFLG1=0xFF;
        TIE=0x00;
      }
    }
    
    else if(status==3)  //设定运行速度
    {
      switch(status_knob)
      {
        case 1:
        {
          maxspeed += 50;
          status_knob=0;
          break;
        }
        case 2:
        {
          maxspeed -= 50;
          status_knob=0;
          break;
        }
        case 3:
        {
          minspeed += 50;
          status_knob=0;
          break;
        }
        case 4:
        {
          minspeed -= 50;
          status_knob=0;
          break;
        }
      }
      
      sprintf(mystr,"%4d",maxspeed);
      LCD_P6x8Str(24,6,mystr);
      sprintf(mystr,"%4d",minspeed);
      LCD_P6x8Str(24,7,mystr);
      
      status_button=check_button();
      if(status_button==1)//按下按钮1回到传感器数值显示阶段
      {
        status=0;
        LCD_P6x8Str(0,0,"Left=");
        LCD_P6x8Str(0,1,"Center=");
        LCD_P6x8Str(0,2,"Right=");
        TFLG1=0xFF;
        TIE=0x00;
      }
      else if(status_button==2)//按下按钮2准备启动
      {
        LCD_P6x8Str(0,0,"GO GO GO!!!");
        wait(120);
        LCD_CLS();
        status++;
      }
    }
    
    else if(status==4)  //小车即将启动，这里完成启动前最后的初始化，并启动电机
    {
      TFLG1=0xFF;
      TIE=0x00;
      /**************************************************************
      这里可能还需要补充一些变量的初始化，另外还需添加电机启动的代码
      **************************************************************/
      
      break;//跳出调试阶段死循环，进入小车运行阶段
    }
  }

}