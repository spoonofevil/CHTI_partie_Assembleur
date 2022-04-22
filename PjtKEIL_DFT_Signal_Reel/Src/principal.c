

#include "DriverJeuLaser.h"
extern int DFT_ModuleAuCarre(short * tab,int i);
extern short LeSignal[];
int res[7];
short dma_buf[64];

void Analyse(){//les frequences qui nous interesse k=17 f=85/18 90/19 95/20 100/23 115/ 24 120
	res[0] = DFT_ModuleAuCarre(dma_buf,21);
	//if res prob
	
	
	res[1] = DFT_ModuleAuCarre(dma_buf,17);
	res[2] = DFT_ModuleAuCarre(dma_buf,18);
	res[3] = DFT_ModuleAuCarre(dma_buf,19);
	res[4] = DFT_ModuleAuCarre(dma_buf,20);
	res[5] = DFT_ModuleAuCarre(dma_buf,23);
	res[6] = DFT_ModuleAuCarre(dma_buf,24);
	
}


void Echantillonage(){
	Start_DMA1(64);
	Wait_On_End_Of_DMA1();
	Stop_DMA1;
	Analyse();
}


int main(void){

// ===========================================================================
// ============= INIT PERIPH (faites qu'une seule fois)  =====================
// ===========================================================================

// Après exécution : le coeur CPU est clocké à 72MHz ainsi que tous les timers
	CLOCK_Configure();
	

	//config timer mesure
	Systick_Period_ff( 360000 );
	Systick_Prio_IT(10 , Echantillonage);
	SysTick_On;
	
	
	
	
	SysTick_Enable_IT;
	

	//configuration de l'echantillonage
	Init_TimingADC_ActiveADC_ff( ADC1, 72);
	Single_Channel_ADC( ADC1, 2 );
	Init_Conversion_On_Trig_Timer_ff( ADC1, TIM2_CC2, 225 );
	Init_ADC1_DMA1( 0, dma_buf );
	
//DFT_ModuleAuCarre(&LeSignal[0],3);
//============================================================================	
	
	
while	(1){
	}
}

