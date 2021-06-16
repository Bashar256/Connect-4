#include <stdio.h>
#include <ctype.h>
#include "unp.h"
#define size 6
#define Max 1024
#define KRED  "\x1B[31m"
#define KBLU  "\x1B[34m"
#define RESET "\x1B[0m"
int i,j,k;
int Board[size][size+1];
char Buffer[Max];
char Player_No[1];
char Choice[1];
void Print_Board()
{
	printf("\n\nBoard: \n");
	k = 0;
	for(i = 0; i < size; i++)
	{
		for (j = 0; j <= size; j++)
		{
			Board[i][j] = Buffer[k] - '0';

			if (Board[i][j] == 0)
				printf("%d\t",Board[i][j]);			
			else if (Board[i][j] == 1)
			{
				printf(KRED "%d\t",Board[i][j]);
				printf(RESET);
			}
			else if (Board[i][j] == 2)
			{
				printf(KBLU "%d\t",Board[i][j]);
				printf(RESET);			
			}

			k++;
		}
		printf("\n");
	}
	printf("\n");

}

int main (int argc, char ** argv)
{
	int Socket_File_Descriptor;
	struct sockaddr_in Server;
	
	if (argc != 2)
		err_quit("usage: a.out <IPaddress>");

	if ( (Socket_File_Descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&Server, size);
	bzero(&Buffer, Max);

	Server.sin_family = AF_INET;
	Server.sin_port   = htons(6969);

	if (inet_pton(AF_INET, argv[1], &Server.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	if (connect(Socket_File_Descriptor, (SA *) &Server, sizeof(Server)) < 0)
		err_sys("connect error");

	Readn(Socket_File_Descriptor, Buffer, Max);
	fputs(Buffer, stdout);
	
	while (1) // the players choice whether to play or not
	{
		scanf("%s",Choice);
		Choice[0] = tolower(Choice[0]);
	
		if (Choice[0] == 'p')
		{
			Writen(Socket_File_Descriptor, Choice, 1);
			break;
		}
		else if (Choice[0] == 'q') 
		{
			Writen(Socket_File_Descriptor, Choice, 1);
			close(Socket_File_Descriptor);
			return 0;
		}
	
		printf("\nEnter a correct choice.\n");
	}
	
	Readn(Socket_File_Descriptor, Player_No, sizeof(Player_No)); //assign a number to each player
	printf("Player%c\n\n",Player_No[0]);
	
	while(1)  //infinte loop  
	{	
		Readn(Socket_File_Descriptor, Buffer, Max);// reads board if not the final statement

		if(!strcmp(Buffer, "turns out you aint retarded.") || !strcmp(Buffer, "You lost.......pathetic.") || !strcmp(Buffer, "No one wins"))
		{	
			fputs(Buffer,stdout);
			printf("\n\n");
			close(Socket_File_Descriptor);
			return 0;
		}
		else 
			Print_Board();	

		Readn(Socket_File_Descriptor, Buffer, Max);// reads the statement that specifies next move
		
		if(!strcmp(Buffer, "turns out you aint retarded.") || !strcmp(Buffer, "You lost.......pathetic.") || !strcmp(Buffer, "No one wins"))
		{	
			fputs(Buffer,stdout);
			printf("\n\n");
			close(Socket_File_Descriptor);
			return 0;
		}
		else 
			fputs(Buffer,stdout);// prints whats your move?	

		scanf("%s",Buffer);

		Writen(Socket_File_Descriptor,Buffer, Max); 

		while (1)//to check the validness of the move
		{
			Readn(Socket_File_Descriptor, Buffer, Max);			

		 	if (!strcmp(Buffer, "full") || !strcmp(Buffer, "invalid"))// whether the move is valid or not
		 	{
			 	Readn(Socket_File_Descriptor, Buffer, Max);//next move
			 	printf("\n");			 	
			 	fputs(Buffer, stdout);
				scanf("%s",Buffer);
			 	printf("\n\n");
			 	Writen(Socket_File_Descriptor, Buffer, Max);
		 	}
		 	else 
		 		break;
		}
		Print_Board();
	}

}
