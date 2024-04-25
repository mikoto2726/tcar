#include <tm/tmonitor.h>
#define PE_DATA 0x400C0400 //PEデータレジスタのアドレス
#define PE_CR   0x400C0404 //PE出力コントロールレジスタ

EXPORT INT usermain(void){
	_UW   i;
	*(_UW*)(PE_CR) |= (1<<2); //PE3出力許可
	*(_UW*)(PE_CR) |= (1<<3); //PE3出力許可
	for (int i=1;i<=3; i++){
		*(_UW*)(PE_DATA) |= (1<<3); //PE3'High'出力
		tk_dly_tsk(500);
		*(_UW*)(PE_DATA) |= (1<<2); //PE2'High'出力
		*(_UW*)(PE_DATA) &= ~(1<<3); //PE3'Low'出力
		tk_dly_tsk(500);
		*(_UW*)(PE_DATA) &= ~(1<<2); //PE2'Low'出力
	}
	return 0;
}