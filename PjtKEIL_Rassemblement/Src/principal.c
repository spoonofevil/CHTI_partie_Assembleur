#include "DriverJeuLaser.h"

//partie son
#include "Son/GestionSon.h"
extern short Son;
extern int LongueurSon;
extern void PlaySound(void);
short* Cursor;


//partie DFT
#define SEUIL 0x20000 //seuil à changer pour detecter qd un tir a touché la cible
//les indices des differents joueurs
#define J1 17
#define J2 18
#define J3 19
#define J4 20
#define J5 23
#define J6 24
//les frequences qui nous interessent k=17 f=85/18 90/19 95/20 100/23 115/ 24 120
extern int DFT_ModuleAuCarre(short * tab,int i);//lien vers DFT en asm
int res[4];//tableau pour stocker les resultats des DFTs 
int restmp[4];
short dma_buf[64];//tableau pour stocker l'échantillonage du signal reçu

//partie score final
#include "ModuleAffichage/Affichage_Valise.h"
#define NBJOUEUR 4
int Score[NBJOUEUR];//pour stocker le score des joueurs
char Tab_LED[NBJOUEUR];//pour pouvoir stocké les differents nom de LED (dans l'ordre que l'on veut)
int LED_a_toucher;//LED à allumer pour signaler au joueur qu'il faut viser sur celle-ci


//fonction pour mettre à jour les LEDs de score
void MAJ_Score_LED(){
	for(int i=1;i<NBJOUEUR+1;i++){
		Prepare_Afficheur(i, Score[i-1]);
		Prepare_Clear_Point_Unite(i);
		
	}
	Prepare_Set_LED(Tab_LED[LED_a_toucher]);
	Prepare_Clear_LED(Tab_LED[(LED_a_toucher+1)%4]);
	Prepare_Clear_LED(Tab_LED[(LED_a_toucher+2)%4]);
	Prepare_Clear_LED(Tab_LED[(LED_a_toucher+3)%4]);
	Mise_A_Jour_Afficheurs_LED();
}
//fonction pour mettre à jour le score et jouer le son
void Touche(int i){
	StartSon();
	Score[i]++;
	LED_a_toucher=(LED_a_toucher+1)%4;
	 MAJ_Score_LED();
}



//fonction permettant de lancer les différentes DFTs sur le signal échantilloné et d'analyser pour savoir qui a touché
void Analyse(){
	res[0] = DFT_ModuleAuCarre(dma_buf,J1);
	res[1] = DFT_ModuleAuCarre(dma_buf,J2);
	res[2] = DFT_ModuleAuCarre(dma_buf,J3);
	res[3] = DFT_ModuleAuCarre(dma_buf,J4);
	for(int i=0;i<4;i++){
		if(res[i]>SEUIL){
			restmp[i]++;
			if (restmp[i] == 5){
				Touche(i);
			}
		} else {
			restmp[i] = 0;
		}
	}
}
//fonction lancé à chaque fin de timer pour lancer l'échantillonage du signal directement dans la RAM
void Echantillonage(){
	Start_DMA1(64);
	Wait_On_End_Of_DMA1();
	Stop_DMA1;
	Analyse();
}
 
//fonction permettant de lancer la lecture d'un son
void StartSon(){
	Cursor = &Son;
}


void Config_Timer_Mesure(){//la periode de mesure doit etre 1/2 du temps de tir
	Systick_Period_ff( 360000 );
	Systick_Prio_IT(10 , Echantillonage);
	SysTick_On;
	
	SysTick_Enable_IT;
}
void Config_Echantillonage(){
	Init_TimingADC_ActiveADC_ff( ADC1, 72);
	Single_Channel_ADC( ADC1, 2 );
	Init_Conversion_On_Trig_Timer_ff( ADC1, TIM2_CC2, 225 );
	Init_ADC1_DMA1( 0, dma_buf );
}
void Config_Score(){
	//config pour le score
	for(int i=0;i<NBJOUEUR;i++){
		Score[i]=0;
	}
	//config pour les LED
	LED_a_toucher=0;
	Tab_LED[0]=LED_Cible_4;
	Tab_LED[1]=LED_Cible_1;
	Tab_LED[2]=LED_Cible_3;
	Tab_LED[3]=LED_Cible_2;
	
}



void Config_Son(){
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
	
	//config le lancement de la mesure d'un signal
	Config_Timer_Mesure();
	
	//configuration de l'echantillonage
	Config_Echantillonage();
	
	//configure le tableau des scores
	Config_Score();
	
	
	Config_Son();
	
	Init_Affichage();
	MAJ_Score_LED();
	
while	(1){
	}
}

