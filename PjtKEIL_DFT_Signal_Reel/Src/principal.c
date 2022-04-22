

#include "DriverJeuLaser.h"
extern int DFT_ModuleAuCarre(short * tab,int i);
extern short LeSignal[];

void Analyse(){
	for(int i=0;i<6;i++){
		int res=DFT_ModuleAuCarre(0,i);
	}
}

short dma_buf[64];

void Echantillonage(){
	int i=0;
	Start_DMA1(64);
	Wait_On_End_Of_DMA1();
	Stop_DMA1;
}


int main(void){

// ===========================================================================
// ============= INIT PERIPH (faites qu'une seule fois)  =====================
// ===========================================================================

// Après exécution : le coeur CPU est clocké à 72MHz ainsi que tous les timers
	CLOCK_Configure();
	
	//configuration de l'echantillonage
	Init_TimingADC_ActiveADC_ff( ADC1, 72);
	Single_Channel_ADC( ADC1, 2 );
	Init_Conversion_On_Trig_Timer_ff( ADC1, TIM2_CC2, 225 );
	Init_ADC1_DMA1( 0, dma_buf );

	//config timer mesure
	Systick_Period_ff( 360000 );
	Systick_Prio_IT(2 , Echantillonage);
	SysTick_On;
	
	
	
	
	SysTick_Enable_IT;
	

//DFT_ModuleAuCarre(&LeSignal[0],3);
//============================================================================	
	
	
while	(1){
	}
}

