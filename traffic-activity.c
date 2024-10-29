#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef char String[0x1F];

typedef struct{
	int priority;
	String trafficMovement;
	int time;
}Traffic;

typedef struct{
	Traffic	traffic;
	int count;
}TrafficPOT;

char* createTestFile();	// returns file name

int main(){
	
	Traffic T[8] = {
		{0, "Main", 10},
		{1, "Diversion", 10},
		{2, "Main left", 30},
		{3, "Diversion left", 30},
		{4, "Main right", 15},
		{5, "Diversion right", 15},
		{6, "Main pedestrian", 20},
		{7, "Diversion pedestrian", 20},
	};
	
	FILE *fp = fopen("traffic.dat", "wb");
	
	if( fp != NULL){
		fwrite(T, sizeof(Traffic), 8, fp);
	}
	
	fclose(fp);
	
	char* testcase = createTestFile();
	printf("%s", testcase);
	return 0;
}

char* createTestFile(){
	char* fileName = (char*)malloc(sizeof(char) * 0x1F);
	strcpy(fileName, "testcase.dat");
	FILE *fp = fopen(fileName, "wb");
	Traffic T[5] = {
		{7, "Diversion pedestrian", 20},
		{1, "Diversion", 10},
		{6, "Main pedestrian", 20},
		{3, "Diversion left", 30},
		{4, "Main right", 15},
	};
	if( fp != NULL){
		fwrite(T, sizeof(Traffic), 5, fp);
	}
	fclose(fp);
	return fileName;
}
