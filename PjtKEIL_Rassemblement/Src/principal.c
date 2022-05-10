#include "DriverJeuLaser.h"
#include "GestionSon.h"

//partie son
extern short Son;
extern int LongueurSon;
extern void PlaySound(void);
short* Cursor;


//partie DFT
#define SEUIL 0x20000 //seuil a changer pour detecter qd un tir a touché la cible
extern int DFT_ModuleAuCarre(short * tab,int i);//lien vers DFT en asm
int res[7];//tableau pour stocké les resultats des DFTs 
short dma_buf[64];//tableau pour stocké l'echantillonage du signal recu



//fonction pour mettre a jour le score et jouer le son
void Touche(int i){

	StartSon();
}
//fonction permettant de lancer les differentes DFTs et d'analyser pour savoir qui a touché
void Analyse(){//les frequences qui nous interesse k=17 f=85/18 90/19 95/20 100/23 115/ 24 120
	res[0] = DFT_ModuleAuCarre(dma_buf,21);
	//if res prob seuil a 0x20000 corespond environ à A=20Vpp
	
	
	res[1] = DFT_ModuleAuCarre(dma_buf,17);
	res[2] = DFT_ModuleAuCarre(dma_buf,18);
	res[3] = DFT_ModuleAuCarre(dma_buf,19);
	res[4] = DFT_ModuleAuCarre(dma_buf,20);
	res[5] = DFT_ModuleAuCarre(dma_buf,23);
	res[6] = DFT_ModuleAuCarre(dma_buf,24);
	for(int i=1;i<7;i++){
		if(res[i]>SEUIL){
			Touche(i);
		}
	}
}


//fonction lancé à chaque fin de timer pour lancer l'échantillonage du signal directement dans la RAM
void Echantillonage(){
	Start_DMA1(64);
	Wait_On_End_Of_DMA1();
	Stop_DMA1;
}
 
//fonction permettant de lancer la lecture d'un son
void StartSon(){
	CLOCK_Configure();
	int Te=72*91;
	Timer_1234_Init_ff(TIM4,Te);
	Active_IT_Debordement_Timer( TIM4, 2, PlaySound );
	PWM_Init_ff( TIM3, 3, 720 );

	GPIO_Configure(GPIOB, 0, OUTPUT, ALT_PPULL);

	Cursor = &Son;
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
	
	
while	(1){
	}
}

