#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include <unistd.h> 
#include <sys/time.h> 
#include <termios.h>

// global defines
#define NB_SOUNDS        12 // to be changed to 48
#define TXT_X_BEGIN      22
#define TXT_Y_MODE       22
#define TXT_Y_SOUND_NUM  25
#define TXT_Y_GUESS      27
#define TXT_Y_VALIDATION 29
#define TXT_Y_CONFIRM    32
#define TXT_Y_PLAY_AGAIN 35

// check if text in stdin buffer
// https://c.developpez.com/faq/?page=Gestion-du-clavier-et-de-l-ecran-en-mode-console
int unix_text_kbhit(void) 
{ 
	struct timeval tv = { 0, 0 }; 
	fd_set readfds; 

	FD_ZERO(&readfds); 
	FD_SET(STDIN_FILENO, &readfds); 

	return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) == 1; 
}

// raw/cooked mode terminal to prevent echo of keyboard
// https://c.developpez.com/faq/?page=Gestion-du-clavier-et-de-l-ecran-en-mode-console
void mode_raw(int activate) 
{
	static struct termios cooked; 
    
	if (activate) 
	{ 
		struct termios raw; 

		tcgetattr(STDIN_FILENO, &cooked); 
		raw = cooked; 
		cfmakeraw(&raw); 
		tcsetattr(STDIN_FILENO, TCSANOW, &raw); 
    }
	else
		tcsetattr(STDIN_FILENO, TCSANOW, &cooked); 
}

// print string at x,y
void print_at(int x, int y, char* str)
{
	printf("\033[%d;%dH", y, x);
	printf("%s", str);
}

// clear text at y starting at TXT_X_BEGIN + put cursor back
void clear_zone(int y) {
	print_at(TXT_X_BEGIN, y, "                                                            ");
	print_at(TXT_X_BEGIN, y, "");
}

// print ansi code to clear screen
void clear_screen()
{
	printf("\033[H\033[2J"); // clear screen
	fflush(stdout);
}

// play sound using play command from sox
void play_sound(char* noise)
{
	char cmd[255] = "play -q mp3/";
	strcat(cmd, noise);
	strcat(cmd, ".mp3 2>/dev/null");
	system(cmd);
}

// choose difficulty of game
int choose_difficulty()
{
	int difficulty = 0;

	clear_screen();
	while (difficulty < 1 || difficulty>3) {
		printf("CHOISIR DIFFICULTE:\n");
		printf("1- FACILE\n");
		printf("2- NORMAL\n");
		printf("3- DIFFICILE\n");
		printf("VOTRE CHOIX: ");
		difficulty = 0;
		if ( scanf("%d", &difficulty) == 0 )
			while((getchar())!='\n');
	}
	return difficulty;
}

// play random sound to guess
void random_sounds(int nbsound, char* emo[][2], int* random)
{
	int num = 0;

	srand(time(NULL));
	for(int i=0; i<nbsound; i++) {
		num = rand() % NB_SOUNDS + 1; 
		random[i] = num;
		play_sound(emo[num][1]);
	}
}

// get answers from user
void get_responses(int nbsound, char* emo[][2], int* answer)
{
	int num = 0;
	char confirm[50] = "NO";

	while (strcmp(confirm,"OUI") != 0) {
		clear_zone(TXT_Y_GUESS);
		for(int i=0; i<nbsound; i++) {
			clear_zone(TXT_Y_SOUND_NUM);
			while ( scanf( "%d", &num ) == 0 || num>NB_SOUNDS || num<1) {
				while((getchar())!='\n');
				clear_zone(TXT_Y_SOUND_NUM);
			}
			answer[i] = num;
			print_at(TXT_X_BEGIN+i*3, TXT_Y_GUESS, emo[num][0]);
		}
		print_at(3, TXT_Y_CONFIRM, "CONFIRMEZ-VOUS VOTRE CLASSEMENT ? ");
		scanf("%s", confirm);
		print_at(3, TXT_Y_CONFIRM, "                                                       ");
	}	
}

// compare random played to guess from user
void compare_guess(int nbsound, char* emo[][2], int* random, int* answer)
{
	bool victory = true;

	for(int i=0; i<nbsound; i++) {
		play_sound(emo[random[i]][1]);
		if (random[i] == answer[i])
			print_at(TXT_X_BEGIN+i*3, TXT_Y_VALIDATION, "✔️ ");
		else {
			print_at(TXT_X_BEGIN+i*3, TXT_Y_VALIDATION, "❌ ");
			victory = false;	
		}
		fflush(stdout);
	}
	if (victory) {
		print_at(TXT_X_BEGIN+nbsound*3, TXT_Y_VALIDATION, "👏");
		play_sound("CLAP");
	} else {
		print_at(TXT_X_BEGIN+nbsound*3, TXT_Y_VALIDATION, "👎");
		play_sound("OLE");
	}
}

// play the game
void play_game(int difficulty)
{
	
	char* emo[][2] = { 
		{ "0" , "NONE" },
		{ "🐸", "GRENOUILLE" },
		{ "🐤", "OISEAU" },
		{ "🐎", "CHEVAL" },
		{ "🐍", "SERPENT" },
		{ "🐱", "CHAT" },
		{ "🐘", "ELEPHANT" },
		{ "🐷", "COCHON" },
		{ "🐑", "MOUTON" },
		{ "🐶", "CHIEN" },
		{ "🐔", "COQ" },
		{ "🦆", "CANARD" },
		{ "🐒", "SINGE" },
		{ "🧯", "EXTINCTEUR" },
		{ "📸", "PHOTO" }, 
		{ "☎", "TELEPHONE" },  
		{ "📺", "TV" },
		{ "📻", "RADIO" }, 
		{ "🔨", "MARTEAU" }, 
		{ "⌚", "MONTRE" }, 
		{ "🚽", "TOILETTE" },
		{ "🚿", "DOUCHE" }, 
		{ "🎥", "PROJECTEUR" }, 
		{ "🧨", "DYNAMITE" },
		{ "💿", "CD" },
		{ "⚽", "FOOTBALL" },
		{ "🏀", "BASKET" },
		{ "🎱", "BILLARD" },
		{ "🏓", "PINGPONG" },
		{ "🏹", "ARC" },
		{ "⛳", "GOLF" },
		{ "🥊", "BOXE" },
		{ "⛸", "PATINAGLACE" },
		{ "🎮", "JEUVIDEO" },
		{ "🏊", "NATATION" },
		{ "🏇", "EQUITATION" },
		{ "🎳", "BOWLING" },
		{ "🚙", "VOITURE" },
		{ "🚂", "LOCOMOTIVE" },
		{ "🚔", "POLICE" },
		{ "🚑", "AMBULANCE" },
		{ "🛵", "SCOOTER" },
		{ "🛹", "SKATE" },
		{ "🚍", "CAMION" },
		{ "🚊", "TRAIN" },
		{ "🚁", "HELICOPTER" },
		{ "✈", "AVION" },
		{ "🚀", "FUSEE" },
		{ "⛴", "BATEAU" }
	};
	char* diffinfo[][2] = {
		{ "NONE", "0" },
		{ "FACILE", "3" },
		{ "NORMAL", "5" },
		{ "DIFFICILE", "10" }
	};
	int nbsound = atoi(diffinfo[difficulty][1]);
	int random[10];
	int answer[10];
    
    clear_screen();
	system("cat emoji2.txt");
	print_at(18, TXT_Y_MODE, diffinfo[difficulty][0]);
	print_at(TXT_X_BEGIN, TXT_Y_SOUND_NUM, "");
	fflush(stdout);

	mode_raw(1);
	random_sounds(nbsound, emo, random);
	while (unix_text_kbhit()) //purge stdin buffer
		getchar();
	mode_raw(0);
	get_responses(nbsound, emo, answer);
	compare_guess(nbsound, emo, random, answer);

}

int main()
{
	int difficulty = choose_difficulty();
	char answer[50] = "OUI";

	while (strcmp(answer, "OUI") == 0) {
		play_game(difficulty);
		print_at(3, TXT_Y_PLAY_AGAIN, "VOULEZ-VOUS REJOUER ? ");
		scanf("%s", answer);
	}
	print_at(0, 35, "");
}
