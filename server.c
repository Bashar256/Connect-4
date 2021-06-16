#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include "unp.h"
#define size 6
#define Max 1024
#define Max_Players 2
int i,j,k,l;
int Temp, Winner = 0;
bool Play[2]={false, false};
char Player_No[1];
int Board[size][size+1]=
	{
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0}
	};
int Board_Count[]={size-1,size-1,size-1,size-1,size-1,size-1,size-1};

char Buffer[Max];
char Choice[1];

void Exeute_Move(int *(Player_No))
{
	 Temp = Buffer[0] - '0';
	if(Board_Count[Temp -1] < 0)
	{
		strcpy(Buffer, "full");
		return;
	}
	Board[Board_Count[Temp-1]][Temp-1] = *(Player_No);
	Board_Count[Temp-1]--;

		Fill_Buffer();
}
void Fill_Buffer()
{
	k = 0;
	for(i = 0; i < size; i++)
		for (j = 0; j <= size; j++)
			{
				Buffer[k] = Board[i][j] + '0';
					k++;
			}
}


int Check_Win()
{
	//Row Win
	for (i = size-1; i >= 0; i--)
	{
		for (j = 0; j <= size-3; j++)
		{
			if (Board[i][j] == 1 && Board[i][j+1] == 1 && Board[i][j+2] == 1 && Board[i][j+3] == 1)
				return 1;		
			else if (Board[i][j] == 2 && Board[i][j+1] == 2 && Board[i][j+2] == 2 && Board[i][j+3] == 2)
				return 2;
		}
	}

	//Column Win
	for (i = 0; i < size-2; i++)
	{
		for (j = size; j >= 0; j--)
		{
			if (Board[i][j] == 1 && Board[i+1][j] == 1 && Board[i+2][j] == 1 && Board[i+3][j] == 1)
				return 1;
			else if (Board[i][j] == 2 && Board[i+1][j] == 2 && Board[i+2][j] == 2 && Board[i+3][j] == 2)
				return 2;
		}
	}

	//Right Diagonal Win
	for (i = size - 1; i >= 3; i--)
	{
		for (j = 0; j <= size-3; j++)
		{
			if (Board[i][j] == 1 && Board[i-1][j+1] == 1 && Board[i-2][j+2] == 1 && Board[i-3][j+3] == 1)
				return 1;		
			else if (Board[i][j] == 2 && Board[i+1][j+1] == 2 && Board[i+2][j+2] == 2 && Board[i+3][j+3] == 2)
				return 2;
		}
	}

	//Left Diagonal Win
	for (i = size-1; i >= 3; i--)
	{
		for (j = size; j >= 3; j--)
		{
			if (Board[i][j] == 1 && Board[i-1][j-1] == 1 && Board[i-2][j-2] == 1 && Board[i-3][j-3] == 1) 
				return 1;	
			if (Board[i][j] == 2 && Board[i-1][j-1] == 2 && Board[i-2][j-2] == 2 && Board[i-3][j-3] == 2)
				return 2;
		}
	}

	if (Check_Full())
		return -1;

	return 0;

}

int Check_Full()
{
	k = 0;
	for(i = 0; i < size; i++)
		for (j = 0; j <= size; j++)
			if (Board[i][j] == 0)
				return 0;
return 1;

}
struct Thread_Sock{
	int File_Descriptor;
	int Player_No;
};


pthread_mutex_t  Mutex = PTHREAD_MUTEX_INITIALIZER;

void * Control (void * arg)
{
	struct Thread_Sock Player = *(struct Thread_Sock*) arg;
	Writen(Player.File_Descriptor, Player_No, sizeof(Player_No)); 
	sleep(0.5);
	while (1)
	{
		sleep(0.2);
		 if (Play[0] && Play[1])
		{
			Winner = Check_Win();			

			if (Winner != 0)
				pthread_exit(NULL);

			pthread_mutex_lock(&Mutex);

			Fill_Buffer();
			Writen(Player.File_Descriptor, Buffer, Max);

			sleep(0.5);
			if (Winner != 0)
			{
				pthread_mutex_unlock(&Mutex);
				pthread_exit(NULL);
			}
			bzero(&Buffer, Max);
			
			strcpy(Buffer, "What's Your Move: ");
			Writen(Player.File_Descriptor, Buffer, Max);

			
			Readn(Player.File_Descriptor, Buffer, Max);
		
			while(1)
			{
				if (Buffer[0] - '0' > 0 && Buffer[0] - '0' < 8)
					Exeute_Move(&Player.Player_No);
				else
					strcpy(Buffer,"Invalid move!!");
				
				Writen(Player.File_Descriptor, Buffer, Max);

				if (!strcmp(Buffer, "full") || !strcmp(Buffer, "invalid"))
				{
					strcpy(Buffer, "Enter a Valid Move: ");
					Writen(Player.File_Descriptor, Buffer,Max);
					Readn(Player.File_Descriptor,Buffer,Max);
				}
				else 
					break;
			}
			pthread_mutex_unlock(&Mutex);
		}
	}
}


int main (int argc, char ** argv)
{
	int Server_File_Descriptor;
	struct sockaddr_in Server;
	
	Server_File_Descriptor = Socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&Server, sizeof(Server));
    bzero(&Buffer, Max);
    Server.sin_family = AF_INET;
    Server.sin_port = htons(6969);
    Server.sin_addr.s_addr = htonl(0);

    Bind(Server_File_Descriptor, (SA*) &Server, sizeof(Server));

    Listen(Server_File_Descriptor, 2);

    pthread_mutex_init(&Mutex, NULL);

    struct Thread_Sock Players[Max_Players];
  	pthread_t Thread_ID[Max_Players];
  	
  	while(1)
  	{
	  	for (l = 0; l < Max_Players; l++)
	  	{
	  		Players[l].File_Descriptor = Accept(Server_File_Descriptor,NULL,NULL);
	  		strcpy(Buffer, "Welcome to the awesome game of Connect4\n\nDescription: Connect4 is a two-player connection game in which the players take turns dropping one colored disc from the top into a seven-column, six-row vertically suspended grid.\nThe pieces fall straight down, occupying the lowest available space within the column.\nThe objective of the game is to be the first to form a horizontal, vertical, or diagonal line of four of one's own discs.\np: to play\nq: to quit\n");
	  		Writen(Players[l].File_Descriptor, Buffer, Max);
			Readn(Players[l].File_Descriptor,Choice, 1);
			if(Choice[0] == 'p')
			{
				Play[l] = true;
		  		Players[l].Player_No = l + 1;
		  		Player_No[0] = (l+1)+'0';
		  		pthread_create(&(Thread_ID[l]), NULL, &Control, &Players[l]);
			}
		
			else
			{
				Play[l] = false;
				l--;
				close(Players[l].File_Descriptor);
			}
		}
		for (i = 0; i <Max_Players;i ++)
			pthread_join(Thread_ID[i],NULL);

		bzero(&Buffer, Max);
		for (i = 0; i < Max_Players; i++)
		{
			if (Winner == -1)
			{
				strcpy(Buffer, "No one wins");
				Writen(Players[i].File_Descriptor, Buffer, Max);
			}
			else if(Winner == Players[i].Player_No)
			{
				strcpy(Buffer, "turns out you aint retarded.");
				Writen(Players[i].File_Descriptor, Buffer, Max);
			}
			else 
			{
				strcpy(Buffer, "You lost.......pathetic.");
				Writen(Players[i].File_Descriptor, Buffer, Max);	
			}

			close(Players[i].File_Descriptor);
		}

		bzero(&Buffer, Max);
  		Play[0] = false;
  		Play[1] = false;
  		Winner = 0;
		for(i = 0; i < size; i++)
			for (j = 0; j <= size; j++)
			{
				Board[i][j] = 0;		
				Board_Count[j] = size-1;
			}
	}
}
