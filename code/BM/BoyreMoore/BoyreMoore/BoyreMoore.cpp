// BoyreMoore.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <string>
#include <conio.h>

using namespace std;

/*char* extractalphabet(char* text){
}
*/
void printtable(int **table,int r,int c){
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++)
			cout<<table[i][j];
		cout<<endl;
	}
}

int** preprocess(char *pattern, char *alphabet){
	int k=strlen(pattern),s=strlen(alphabet);
	//initalize table with all zeros
	int** table = new int* [k];
	char *part = new char [k];
    for (int i = 0; i < k; i++){
		table[i] = new int[s];
		part[k]='\0';
		for (int j=0; j<s; j++)
			table[i][j]=0;	
	}
	//update table based on bad character rule
    for(int i=1;i<k;i++)
		for(int j=0;j<s;j++){
			table[i][j]=table[i-1][j];
			if(pattern[i-1]==alphabet[j]){
				strncpy (part,pattern,i);
				table[i][j]=strrchr(part,alphabet[j])-part+1;		
			}
		}
	//printtable(table,k,s);
	return table;
}

int bmsearch(char *text, char *pattern, int** table){
	int n=strlen(text),k=strlen(pattern);
	
	return 0;
}

void boyremoore(char *text, char *pattern,char *alphabet){
	int occ=0;
	int n=strlen(text),k=strlen(pattern),s=strlen(alphabet);

	int **table;

	cout<<"BM preprocessing pattern...\n";
	table=preprocess(pattern,alphabet);

	cout<<"BM searching pattern...\n";
	occ=bmsearch(text,pattern,table);
	cout<<"BM foud "<<occ<<" occurences";
}

int main(int argc, _TCHAR* argv[])
{
	char *text="GCATCGCAFAFAFTATACAGTACG";
	char *pattern="GCAGAGAG";
	char *alphabet="ACGT";//extractalphabet(text);
	boyremoore(text,pattern,alphabet);
	getch();
	return 0;
}

