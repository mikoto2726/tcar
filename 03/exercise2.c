#include <tk/tkernel.h>
#include <tm/tmonitor.h>

// GPIOポートH
#define GPIOH_BASE (0x400C0700)
#define PHCR    (GPIOH_BASE + 0x04)
#define PHFR3   (GPIOH_BASE + 0x10)
#define PHIE    (GPIOH_BASE + 0x38)

// MPT2タイマ
#define	MT2_BASE	(0x400C7200UL)
#define	MT2EN   (MT2_BASE + 0x00)
#define	MT2RUN	(MT2_BASE + 0x04)
#define	MT2IGCR	(MT2_BASE + 0x30)
#define	MT2IGOCR	(MT2_BASE + 0x40)
#define	MT2IGRG2	(MT2_BASE + 0x44)
#define	MT2IGRG3	(MT2_BASE + 0x48)
#define	MT2IGRG4	(MT2_BASE + 0x4C)


EXPORT	INT	usermain( void )
{
  // MTP2初期設定
  *(_UW*)MT2EN     |= (1<<7) | (1<<0);
  *(_UW*)MT2IGOCR  |= (1<<1) | (0<<5);
  *(_UW*)MT2IGCR   &= ~((1<<6) | (1<<2) | (1<<3) | (1<<0) | (1<<1));  // 0b10110000

  // 端子をMTP2に設定
  *(_UW*)PHFR3   |=  (1<<2);
  *(_UW*)PHIE    &=  ~(1<<2);
  *(_UW*)PHCR    |=  (1<<2);
  
  // MTP2 出力波形の設定 ド
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 17200;
  *(_UW*)MT2IGRG3  = 8600;

  *(_UW*)MT2RUN    |= (1<<2) | (1<<0);   // ブザー鳴動開始
  
  tk_dly_tsk(500);

  *(_UW*)MT2RUN    &= ~((1<<2) | (1<<0));  // ブザー鳴動停止

  // MTP2 出力波形の設定 レ
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 15300; // 426 ÷ 0.02778
  *(_UW*)MT2IGRG3  = 7650;

  *(_UW*)MT2RUN    |= (1<<2) | (1<<0);   // ブザー鳴動開始
  
  tk_dly_tsk(500);

  *(_UW*)MT2RUN    &= ~((1<<2) | (1<<0));  // ブザー鳴動停止

  // MTP2 出力波形の設定 ミ
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 13600;
  *(_UW*)MT2IGRG3  = 6800;

  *(_UW*)MT2RUN    |= (1<<2) | (1<<0);   // ブザー鳴動開始
  
  tk_dly_tsk(500);

  *(_UW*)MT2RUN    &= ~((1<<2) | (1<<0));  // ブザー鳴動停止

  // MTP2 出力波形の設定 ファ
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 12800;
  *(_UW*)MT2IGRG3  = 6400;

  *(_UW*)MT2RUN    |= (1<<2) | (1<<0);   // ブザー鳴動開始
  
  tk_dly_tsk(500);

  *(_UW*)MT2RUN    &= ~((1<<2) | (1<<0));  // ブザー鳴動停止

  // MTP2 出力波形の設定 ソ
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 11500;
  *(_UW*)MT2IGRG3  = 5700;

  *(_UW*)MT2RUN    |= (1<<2) | (1<<0);   // ブザー鳴動開始
  
  tk_dly_tsk(500);

  *(_UW*)MT2RUN    &= ~((1<<2) | (1<<0));  // ブザー鳴動停止

  // MTP2 出力波形の設定 ラ
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 10200;
  *(_UW*)MT2IGRG3  = 5100;

  *(_UW*)MT2RUN    |= (1<<2) | (1<<0);   // ブザー鳴動開始
  
  tk_dly_tsk(500);

  *(_UW*)MT2RUN    &= ~((1<<2) | (1<<0));  // ブザー鳴動停止

  // MTP2 出力波形の設定 シ
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 9100;
  *(_UW*)MT2IGRG3  = 4550;

  *(_UW*)MT2RUN    |= (1<<2) | (1<<0);   // ブザー鳴動開始
  
  tk_dly_tsk(500);

  *(_UW*)MT2RUN    &= ~((1<<2) | (1<<0));  // ブザー鳴動停止

  // MTP2 出力波形の設定 ド
  *(_UW*)MT2IGRG2  = 1;
  *(_UW*)MT2IGRG4  = 8600;
  *(_UW*)MT2IGRG3  = 4300;

  *(_UW*)MT2RUN    |= (1<<2) | (1<<0);   // ブザー鳴動開始
  
  tk_dly_tsk(500);

  *(_UW*)MT2RUN    &= ~((1<<2) | (1<<0));  // ブザー鳴動停止

  return 0; 
}

