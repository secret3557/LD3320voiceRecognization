#include "ld3320.h"


uint8_t g_Mic;
int RSTB=9;//RSTB竘褐隅砱
int CS=4;//RSTB竘褐隅砱
uint8_t MIC_VOL=0x55;//ADC崝祔場宎硉
uint8_t speech_endpoint=0x10;//逄秞傷萸潰聆場宎硉
uint8_t speech_start_time=0x08;//逄秞傷萸潰聆羲宎奀潔場宎硉
uint8_t speech_end_time=0x10;//逄秞傷萸潰聆賦旰奀潔場宎硉
uint8_t voice_max_length=0xC3;//郔酗逄秞僇奀潔ㄛ蘇20鏃
uint8_t noise_time=0x02;//綺謹奻萇婑汒奀潔
//uint8_t ASR_time郔酗奀潔
int readflag=0;
int readnum=0;

VoiceRecognition::VoiceRecognition(){}

int VoiceRecognition::read()//妎梗賦彆黍
{
	if(readflag==1)
	{	
		readflag=0;
		return readnum;
	}
	return -1;
}
void update()//笢剿督昢滲杅
{ 
	uint8_t Asr_Count=0;
	if((readReg(0x2b) & 0x10) && readReg(0xb2)==0x21 && readReg(0xbf)==0x35)//彆衄逄秞妎梗笢剿﹜DSP玿﹜ASR淏都賦旰
	{
		writeReg(0x29,0) ;///////////壽笢剿
		writeReg(0x02,0) ;/////////////壽FIFO笢剿
		Asr_Count = readReg(0xba);//黍笢剿落翑陓洘
		if(Asr_Count>0 && Asr_Count<4) //////彆衄妎梗賦彆
		{
			readnum=readReg(0xc5);
			readflag=1;
		}	
		writeReg(0x2b,0);//////奠笢剿晤瘍
		writeReg(0x1C,0);////////簷侔壽闔親瑞陛~~峈禱
	}
	readReg(0x06);  
	delay(10);
	readReg(0x06);  
	writeReg(0x89, 0x03);  
	delay(5);
	writeReg(0xcf, 0x43);  
	delay(5);
	writeReg(0xcb, 0x02);
	writeReg(0x11, PLL_11);  
	writeReg(0x1e,0x00);
	writeReg(0x19, PLL_ASR_19); 
	writeReg(0x1b, PLL_ASR_1B);	
	writeReg(0x1d, PLL_ASR_1D);
	delay(10);
	writeReg(0xcd, 0x04);
	writeReg(0x17, 0x4c); 
	delay(5);
	writeReg(0xcf, 0x4f);  
	writeReg(0xbd, 0x00);
	writeReg(0x17, 0x48);
	delay(10);
	writeReg(0x3c, 0x80);  
	writeReg(0x3e, 0x07);
	writeReg(0x38, 0xff);  
	writeReg(0x3a, 0x07);
	writeReg(0x40, 0);   
	writeReg(0x42, 8);
	writeReg(0x44, 0); 
	writeReg(0x46, 8); 
	delay(1);	
	writeReg(0x1c, 0x09);////////闔親瑞扢离悵隱
	writeReg(0xbd, 0x20);/////////悵隱扢离
	writeReg(0x08, 0x01);///////////↙壺FIFO_DATA
	delay( 1);
	writeReg(0x08, 0x00);////////////壺硌隅FIFO綴婬迡珨棒00H
	delay( 1);
	writeReg(0xb2, 0xff);////////跤0xB2迡FF
	writeReg(0x37, 0x06);////////羲宎妎梗
	delay( 5 );
	writeReg(0x1c, g_Mic);////////恁寁闔親瑞
	writeReg(0x29, 0x10);////////羲肮祭笢剿
	writeReg(0xbd, 0x00);/////////雄峈逄秞妎梗
}	
void cSHigh() {//CS嶺詢
  digitalWrite(CS, HIGH);
}
void cSLow() {//CS褐嶺腴
  digitalWrite(CS, LOW);
}
void writeReg(unsigned char address,unsigned char value)////////迡敵湔ㄛ統杅ㄗ敵湔華硊ㄛ杅擂ㄘ
{	
	cSLow();////嶺腴CS
	delay(10);
	transfer(0x04);////////////迡硌鍔
	transfer(address);
	transfer(value);
	cSHigh();////嶺詢CS
 }

unsigned char readReg(unsigned char address)///黍敵湔ㄛ統杅ㄗ敵湔華硊ㄘ
{ 
	unsigned char result;
	cSLow();////嶺腴CS
	delay(10);
	transfer(0x05);///////////黍硌鍔
	transfer(address);
	result=transfer(0x00);
	cSHigh();///嶺詢CS
	return(result);
 }
byte transfer(byte _data) /////////////////SPI杅擂蝠遙
{
	SPDR = _data;
	while (!(SPSR & _BV(SPIF)));
	return SPDR;
}

void VoiceRecognition::init(uint8_t mic)////////耀輸蚚ㄛ統杅峈闔親瑞恁寁ㄗMIC/MONOㄘ迵佪荂勤桽,婓SETUP笢覃蚚
{
	if(mic==MIC)
	{
		g_Mic=MIC;
	}
	else if(mic==MONO)
	{
		g_Mic=MONO;
	}
	pinMode(RSTB,OUTPUT);
	pinMode(CS, OUTPUT);
	cSHigh();

	pinMode(SPI_MISO_PIN, INPUT);
	pinMode(SPI_MOSI_PIN, OUTPUT);
	pinMode(SPI_SCK_PIN, OUTPUT);

	#ifndef SOFTWARE_SPI
	  // SS must be in output mode even it is not chip select
	  pinMode(LD_CHIP_SELECT_PIN, OUTPUT);
	  digitalWrite(LD_CHIP_SELECT_PIN, HIGH); // disable any SPI device using hardware SS 嶺詢ss
	  // Enable SPI, Master, clock rate f_osc/128
	  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);/////場宎趙SPI敵湔
	  // clear double speed
	  SPSR &= ~(1 << SPI2X);//2捷厒
	#endif  // SOFTWARE_SPI	

	SPCR = (SPCR & ~SPI_MODE_MASK) | 0x08;//扢离SCK都怓萇迵欴奀潔ㄛ0x08峈SCK都怓峈詢萇ㄛ狟蔥朓衄虴
	reset();//LD3320葩弇紱釬
	
	#if defined(__AVR_ATmega32U4__)
		attachInterrupt(1,update,LOW);//羲笢剿	
	#else
		attachInterrupt(0,update,LOW);//羲笢剿	
	#endif
	
	ASR_init();///逄秞妎梗場宎趙滲杅	
}

void VoiceRecognition::reset()//LD3320葩弇紱釬
{
  digitalWrite(RSTB,HIGH);
  delay(1);
  digitalWrite(RSTB,LOW);
  delay(1);
  digitalWrite(RSTB,HIGH);
  delay(1);
  cSLow();
  delay(1);
  cSHigh();
  delay(1);  
  writeReg(0xb9, 0x00);
}

void VoiceRecognition::ASR_init()////////////場宎趙逄秞妎梗耀宒﹜
{
	  //氝樓袨怓梓暮 
	  readReg(0x06);  
//	  writeReg(0x17, 0x35); 
	  delay(10);
	  readReg(0x06);  
	  writeReg(0x89, 0x03);  
	  delay(5);
	  writeReg(0xcf, 0x43);  
	  delay(5);
	  writeReg(0xcb, 0x02);
	  writeReg(0x11, PLL_11);  
	  writeReg(0x1e,0x00);
	  writeReg(0x19, PLL_ASR_19); 
	  writeReg(0x1b, PLL_ASR_1B);	
	  writeReg(0x1d, PLL_ASR_1D);
	  delay(10);
	  writeReg(0xcd, 0x04);
	  writeReg(0x17, 0x4c); 
	  delay(5);
//	  writeReg(0xb9, 0x00);
	  writeReg(0xcf, 0x4f);  
	  writeReg(0xbd, 0x00);
	  writeReg(0x17, 0x48);
	  delay(10);
	  writeReg(0x3c, 0x80);  
	  writeReg(0x3e, 0x07);
	  writeReg(0x38, 0xff);  
	  writeReg(0x3a, 0x07);
	  writeReg(0x40, 0);   
	  writeReg(0x42, 8);
	  writeReg(0x44, 0); 
	  writeReg(0x46, 8); 
	  delay(1);
}
void VoiceRecognition::addCommand(char *pass,int num)
{

	int i;
		writeReg(0xc1, num);//趼睫晤瘍
		writeReg(0xc3, 0 );//氝樓奀怀00
		writeReg(0x08, 0x04);//祥壺
  
		delay(1);
		writeReg(0x08, 0x00);//
		delay(1);
	for(i=0;i<=80;i++)
	{
		if (pass[i] == 0)
			break;
		writeReg(0x5, pass[i]);///迡FIFO_EXT
	}
	writeReg(0xb9, i);//迡絞氝樓趼睫揹酗僅
	writeReg(0xb2, 0xff);//////////B2迡ff 
	writeReg(0x37, 0x04);//氝樓逄曆
}
unsigned char VoiceRecognition::start()//////羲宎妎梗
{
	writeReg(0x35, MIC_VOL);////adc崝祔˙頗荌砒妎梗毓峓撈婑汒
	
	writeReg(0xb3, speech_endpoint);//逄秞傷萸潰聆諷秶

	writeReg(0xb4, speech_start_time);//逄秞傷萸宎奀潔
	
	writeReg(0xb5, speech_end_time);//逄秞賦旰奀潔

	writeReg(0xb6, voice_max_length);//逄秞賦旰奀潔
	
	writeReg(0xb7, noise_time);//婑汒奀潔
	
	writeReg(0x1c, 0x09);////////闔親瑞扢离悵隱

	writeReg(0xbd, 0x20);/////////悵隱扢离
	writeReg(0x08, 0x01);///////////↙壺FIFO_DATA
	delay( 1);
	writeReg(0x08, 0x00);////////////壺硌隅FIFO綴婬迡珨棒00H
	delay( 1);
	if(check_b2() == 0)////////黍0xB2敵湔滲杅彆DSP羶婓玿袨怓寀RETURN 0
	{
		return 0;
	}
	writeReg(0xb2, 0xff);////////跤0xB2迡FF

	writeReg(0x37, 0x06);////////羲宎妎梗
	delay( 5 );
	writeReg(0x1c, g_Mic);////////恁寁闔親瑞
	writeReg(0x29, 0x10);////////羲肮祭笢剿
	writeReg(0xbd, 0x00);/////////雄峈逄秞妎梗

	return 1;////殿隙1
}

int check_b2()////////蚚釬潰聆郋馱釬岆瘁淏都ㄛ麼氪DSP岆瘁疆ㄛ祥剒蚚誧紱釬ㄛ淏都/玿殿隙1
{ 
  for (int j=0; j<10; j++)
	{
	  if (readReg(0xb2) == 0x21)
		{
			return 1;
		}
	  delay(10);		
	}
  return 0;
}

void VoiceRecognition::micVol(uint8_t vol)//覃淕ADC崝祔ㄛ統杅ㄗ0x00~0xFF,膘祜10-60ㄘ˙
{
	MIC_VOL=vol;
	writeReg(0x35, MIC_VOL);////adc崝祔˙頗荌砒妎梗毓峓撈婑汒
}
void VoiceRecognition::speechEndpoint(uint8_t speech_endpoint_)//覃淕逄秞傷萸潰聆ㄛ統杅ㄗ0x00~0xFF,膘祜10-40ㄘ˙
{
	speech_endpoint=speech_endpoint_;
	writeReg(0xb3, speech_endpoint);//逄秞傷萸潰聆諷秶
}

void VoiceRecognition::speechStartTime(uint8_t speech_start_time_)//覃淕逄秞傷萸宎奀潔ㄛ統杅ㄗ0x00~0x30,等弇10MSㄘ˙
{
	speech_start_time=speech_start_time_;
	writeReg(0xb4, speech_start_time);//逄秞傷萸宎奀潔
}
void VoiceRecognition::speechEndTime(uint8_t speech_end_time_)//覃淕逄秞傷萸賦旰奀潔ㄗ苂趼潔路奀潔ㄘㄛ統杅ㄗ0x00~0xC3,等弇10MSㄘ˙
{
	speech_end_time=speech_end_time_;
	writeReg(0xb5, speech_end_time);//逄秞賦旰奀潔
}
void VoiceRecognition::voiceMaxLength(uint8_t voice_max_length_)//郔酗逄秞僇奀潔ㄛ統杅ㄗ0x00~0xC3,等弇100MSㄘ˙
{
	voice_max_length=voice_max_length_;
	writeReg(0xb6, voice_max_length);//逄秞
}
void VoiceRecognition::noiseTime(uint8_t noise_time_)//奻萇婑汒謹徹ㄛ統杅ㄗ0x00~0xff,等弇20MSㄘ˙
{
	noise_time=noise_time_;
	writeReg(0xb7, noise_time);//婑汒奀潔
}
