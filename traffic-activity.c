#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX_STRING 0x1F
#define MAX_SCENARIO_STRING 0xFF
#define TRAFFIC_SIZE 0x8
#define MAX_SCENARIO 0x5
#define MAX_SCENARIO_LIST 0x3

typedef char String[MAX_STRING];
typedef char ScenarioString[MAX_SCENARIO_STRING];

typedef struct{
	int priority;
	String trafficMovement;
	int time;
}Traffic;

typedef struct{
	Traffic	*traffic;
	int count;
}TrafficPOT;

typedef struct{
	Traffic *scenario;
	Traffic waiting;
}TrafficScenarios;

typedef struct{
	TrafficPOT scenario;
	Traffic waiting;
	int waitTime;
}TrafficResult;

void trafficToFile(Traffic *A);
void createTestFile(String fileName);
void minHeapifyFile(TrafficPOT *A, String fileName);	// retrieves from file and converts to minheap
void minHeapifyArray(TrafficPOT *P, Traffic *T);
void swapTraffic(Traffic *A, Traffic *B);
Traffic deleteMin(TrafficPOT *A);
void displayScenario(TrafficPOT A);
int getWaitTime(TrafficPOT P, Traffic A);
char* randomScenarioGenerator(Traffic *T);
void scenariosToCSV(Traffic *T, int amount, String fileName);		// returns an array of scenarios based on the passed parameter
TrafficScenarios *parseCSVScenario(String fileName);
TrafficResult *getResults(TrafficScenarios* scenarios);
void resultsToFile(TrafficResult *results, String fileName);
void displayResultsFromFile(String fileName);

int main(){
	/*Activity 1*/
	printf("ACTIVITY 1:\n\n");
	
	// traffic priority & time
	Traffic T[TRAFFIC_SIZE] = {
		{0, "Main", 10},
		{1, "Diversion", 10},
		{2, "Main left", 30},
		{3, "Diversion left", 30},
		{4, "Main right", 15},
		{5, "Diversion right", 15},
		{6, "Main pedestrian", 20},
		{7, "Diversion pedestrian", 20},
	};
	trafficToFile(T);
	
	// create a test file scenario for main pedestrian
	createTestFile("testcase.dat");
	
	// transfer data from file to minheap
	TrafficPOT TP;
	minHeapifyFile(&TP, "testcase.dat");
	
	// get total time for main pedestrian
	Traffic mainPedestrian = T[6];
	displayScenario(TP);
	int totalTime = getWaitTime(TP, mainPedestrian);
	if(totalTime > 0){
		printf("\nTotal wait time before %s can pass the intersection: %ds\n\n", mainPedestrian.trafficMovement, totalTime);
	} else {
		printf("\n%s was not found in the scenario.\n\n", mainPedestrian.trafficMovement);
	}
	
	
	system("pause");
	system("cls");
	
	
	/*Activity 2*/
	printf("ACTIVITY 2:\n\n");
	
	// creates random generated scenarios and stores them in a csv file
	scenariosToCSV(T, 3, "traffic_scenarios.csv");
	
	// parse csv to TrafficScenarios
	parseCSVScenario("traffic_scenarios.csv");
	TrafficScenarios *scenarios = parseCSVScenario("traffic_scenarios.csv");
	
	// passes array of scenarios and gets traffic results of passed scenarios
	TrafficResult *results = getResults(scenarios);
	
	// transfers array of results to file
	resultsToFile(results, "traffic_result.dat");
	
//	displayResultsFromFile("traffic_result.dat");
	return 0;
}

void trafficToFile(Traffic *A){
	FILE *fp = fopen("traffic.dat", "wb");
	
	if( fp != NULL){
		fwrite(A, sizeof(Traffic), TRAFFIC_SIZE, fp);
	}
	
	fclose(fp);
}

void createTestFile(String fileName){
	Traffic T[MAX_SCENARIO] = {
		{7, "Diversion pedestrian", 20},
		{1, "Diversion", 10},
		{6, "Main pedestrian", 20},
		{3, "Diversion left", 30},
		{4, "Main right", 15},
	};
	
	FILE *fp = fopen(fileName, "wb");
	if( fp != NULL){
		fwrite(T, sizeof(Traffic), MAX_SCENARIO, fp);
	}
	fclose(fp);
}

void minHeapifyFile(TrafficPOT *A, String fileName){
	FILE *fp = fopen(fileName, "rb");
	A->traffic = (Traffic*)malloc(sizeof(Traffic)*MAX_SCENARIO);
	Traffic temp;
	int ctr = 0;
	
	if(fp != NULL){
		while(fread(&temp, sizeof(Traffic), 1, fp)){
			A->traffic[ctr++] = temp;
			int child = ctr-1;
			int parent = (child-1)/2;
			while(child > 0 && A->traffic[parent].priority > A->traffic[child].priority){
				swapTraffic(&A->traffic[parent], &A->traffic[child]);
				child = parent;
				parent = (child-1)/2;
			}
		}
	}
	A->count = ctr;
	fclose(fp);
}

void minHeapifyArray(TrafficPOT *A, Traffic *T){
	A->traffic = (Traffic*)malloc(sizeof(Traffic)*MAX_SCENARIO);
	Traffic temp;
	int ctr = 0, i;
	for(i = 0; i < MAX_SCENARIO; i++){
		A->traffic[ctr++] = T[i];
		int child = ctr-1;
		int parent = (child-1)/2;
		while(child > 0 && A->traffic[parent].priority > A->traffic[child].priority){
			swapTraffic(&A->traffic[parent], &A->traffic[child]);
			child = parent;
			parent = (child-1)/2;
		}
	}
	A->count = ctr;
}

void swapTraffic(Traffic *A, Traffic *B){
	Traffic temp;
	
	temp = *A;
	*A = *B;
	*B = temp;
}

Traffic deleteMin(TrafficPOT *A){
	Traffic retTraffic;
	
	if(A->count > 0){
		retTraffic = A->traffic[0];
		A->traffic[0] = A->traffic[--A->count];
		int parent = 0;
		int LC = (parent*2) + 1;
		int RC = LC + 1;
		int smallestChild = A->traffic[LC].priority > A->traffic[RC].priority ? RC : LC;
		int lastNdx = A->count - 1;
		while(parent < lastNdx && A->traffic[parent].priority > A->traffic[smallestChild].priority){
			if(A->traffic[parent].priority > A->traffic[smallestChild].priority){
				swapTraffic(&A->traffic[parent], &A->traffic[smallestChild]);
			}
			int LC = (parent*2) + 1;
			int RC = LC + 1;
			int smallestChild = A->traffic[LC].priority > A->traffic[RC].priority ? RC : LC;
			parent = smallestChild;
		}
	} else {
		retTraffic.priority = -1;	// sentinel value
	}
	
	return retTraffic;
}
void displayScenario(TrafficPOT A){
	int i;
	
	printf("%-10s %-20s %-20s\n", "Priority", "Movement", "Time");
	for(i = 0; i < A.count; i++){
		printf("%-10d %-20s %ds\n", A.traffic[i].priority, A.traffic[i].trafficMovement, A.traffic[i].time);
	}
}

int getWaitTime(TrafficPOT P, Traffic A){
	Traffic deleted = deleteMin(&P);
	int totalWaitTime = 0;
	Traffic temp[MAX_SCENARIO];
	int ctr = 0;
	
	while(deleted.priority != -1 && deleted.priority != A.priority){
		totalWaitTime += deleted.time;
		temp[ctr++] = deleted;
		deleted = deleteMin(&P);
	}
	
	FILE *fp = fopen("deleted.dat", "wb");
	if(fp != NULL){
		fwrite(temp, sizeof(Traffic), ctr, fp);
	}
	fclose(fp);
	
	return (deleted.priority == A.priority && strcmp(deleted.trafficMovement, A.trafficMovement) == 0) ? totalWaitTime : -1;
}

char* randomScenarioGenerator(Traffic *T){
	char* scenario = (char*)malloc(sizeof(char) * MAX_STRING);
	scenario[0] = '\0';
	String temp;
	int ndx = rand()%8;
	
	itoa(T[ndx].priority,temp, 10);
	strcat(scenario, temp);
	strcat(scenario, ",");
	strcat(scenario, T[ndx].trafficMovement);
	strcat(scenario, ",");
	itoa(T[ndx].time,temp, 10);
	strcat(scenario, temp);
	strcat(scenario, ",");
	
	return scenario;
}

void scenariosToCSV(Traffic *T, int amount, String fileName){
	srand(time(NULL));
	int i, j;
	ScenarioString *scenarios = (ScenarioString*)malloc(MAX_SCENARIO_STRING*amount);
	
	for(i = 0; i < amount; i++){
		strcpy(scenarios[i],"");
		for(j = 0; j < MAX_SCENARIO; j++){
			strcat(scenarios[i], randomScenarioGenerator(T));
		}
//		printf("%s\n", scenarios[i]);
	}
	
	FILE* fp = fopen(fileName, "w");
	int totalStringSize = 0;
	for(i = 0; i < amount; i++){
		fprintf(fp, "%s", scenarios[i]);
		fprintf(fp, "%s", randomScenarioGenerator(T));
		fprintf(fp, "\n");
	}
	fclose(fp);
}

TrafficScenarios *parseCSVScenario(String fileName){
	TrafficScenarios *scenarios = (TrafficScenarios*)malloc(sizeof(TrafficScenarios)*MAX_SCENARIO_LIST);
	FILE *fp = fopen(fileName, "r");
	int ctr = 0, i = 0, j = 0;
    if (fp != NULL) {
        ScenarioString line;
	    while (fgets(line, sizeof(line), fp) && i < MAX_SCENARIO_LIST) {
	    	scenarios[i].scenario = (Traffic*)malloc(sizeof(Traffic)*MAX_SCENARIO);
	    	char *token = strtok(line, ",");
	    	j = ctr = 0;
	    	while (token != NULL && j <= MAX_SCENARIO) {
    			switch(ctr){
    				case 0:
    					if(j < MAX_SCENARIO){
    						scenarios[i].scenario[j].priority = atoi(token);
						} else {
							scenarios[i].waiting.priority = atoi(token);
						}
						break;
					case 1:
						if(j < MAX_SCENARIO){
    						strcpy(scenarios[i].scenario[j].trafficMovement, token);
						} else {
							strcpy(scenarios[i].waiting.trafficMovement, token);
						}
						break;
					case 2:
						if(j < MAX_SCENARIO){
    						scenarios[i].scenario[j].time = atoi(token);
						} else {
							scenarios[i].waiting.time = atoi(token);
						}
						ctr = -1;
						j++;
						break;
					default:
						break;
				}
				ctr++;
	            token = strtok(NULL, ",");
	        }
	        i++;
	    }
    }
    fclose(fp);
    return scenarios;
}

TrafficResult *getResults(TrafficScenarios* scenarios){
	TrafficResult *results = (TrafficResult*)malloc(sizeof(TrafficResult)*MAX_SCENARIO_LIST);
	int i;
	for(i = 0; i < MAX_SCENARIO_LIST; i++){
		results[i].scenario.traffic = (Traffic*)malloc(sizeof(Traffic) * MAX_SCENARIO);
		results[i].waiting = scenarios[i].waiting;
		results[i].scenario.count = 0;
		minHeapifyArray(&results[i].scenario, scenarios[i].scenario);
		printf("Waiting: %s\n", results[i].waiting.trafficMovement);
		displayScenario(results[i].scenario);
		results[i].waitTime = getWaitTime(results[i].scenario, results[i].waiting);
		if(results[i].waitTime >= 0){
			printf("\nTotal wait time before %s can pass the intersection: %ds\n\n", results[i].waiting.trafficMovement, results[i].waitTime);
		} else {
			printf("\n%s was not found in the scenario.\n\n", results[i].waiting.trafficMovement);
		}
	}
	return results;
}

void resultsToFile(TrafficResult *results, String fileName){
	FILE *fp = fopen(fileName, "wb");
	fwrite(results, sizeof(TrafficResult),MAX_SCENARIO_LIST, fp);
	fclose(fp);
}








