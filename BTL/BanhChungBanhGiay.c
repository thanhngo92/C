#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MPI 3.1415926535
#define MAX_NEP 2000
#define MAX_LD 600
#define DEBUG 0  

typedef struct NguyenLieu {
    int n;   
    int dc;  
    int dg;  
    int ld; 
    char w[10];
} NL;

typedef struct ThanhPham {
    int bc;    
    int bg;    
    float nd; 
} TP;

int isFromSun = 0;
int isFibonacci(int x);
int tongUoc(int n);
void toLowerCase(NL *input);
int kiem_tra_dv(const NL *input);
float soNepBC(const NL *input);
float soNepBG(const NL *input);
int soLD(const NL *input, int bc, int bg);
void pg_Rain(NL *input, TP *output);
void pg_Wind(NL *input, TP *output);
void pg_Sun(NL *input, TP *output);
void pg_Fog(NL *input, TP *output);
void pg_Cloud(NL *input, TP *output);
void write_output(const char *filename, const NL *input, const TP *output);
void pg_output(const char *inputFile, const char *outputFile);
int main() {
    pg_output("input.inp", "output.out");
    return 0;
}

int tongUoc(int p) {
    if (p <= 0) return 0;
    int i, sum = 0;
    for (i = 1; i <= sqrt(p); i++) {
        if (p % i == 0) {
            sum += i;
            if (i != p / i && i != 1) {
                sum += p / i;
            }
        }
    }
    return sum;
}

int isFibonacci(int xS) {
    if (xS < 0) return 0;
    int n1 = 5 * xS * xS + 4;
    int n2 = 5 * xS * xS - 4;
    int sqrt1 = (int)sqrt(n1);
    int sqrt2 = (int)sqrt(n2);
    return (sqrt1 * sqrt1 == n1) || (sqrt2 * sqrt2 == n2);
}

int isFriendPair(int u, int y) {
    if (u <= 0 || y <= 0 || u == y) return 0;
    int sumU = tongUoc(u);
    int sumY = tongUoc(y);
    return (sumU == y && sumY == u);
}

int serSun() {
    return !isFromSun;
}


void toLower(NL* input) {
    int i;
    for (i = 0; input->w[i]; i++) {
        input->w[i] = tolower(input->w[i]);
    }
}

int kiem_tra_dv(const NL *input) {
    if (input->n <= 0 || input->n > MAX_NEP) return 0;
    if (input->ld <= 0 || input->ld > MAX_LD) return 0;
    if (input->dc < 0 || input->dg < 0) return 1;
    return 1;
}

float soNepBC(const NL *input) {
    return pow(input->dc, 2) * sqrt(input->dc);
}

float soNepBG(const NL *input) {
    return (pow(input->dg, 2) * MPI) / 3;
}


int soLD(const NL *input, int bc, int bg) {
    if (!input || bc < 0 || bg < 0) return 0;
    int ldBC = (input->dc < 8) ? 1 : 2;
    int ldBG = (input->dg < 5) ? 1 : 2; 
    return (bc * ldBC) + (bg * ldBG);
}

void pg_Wind(NL* input, TP* output) {
    if (!serSun() && !kiem_tra_dv(input)) { 
        output->bc = -1;
        output->bg = -1;
        output->nd = input->n;
        return;
    }
	
    if (input->dc <= 0 && input->dg <= 0) {
    	output->bc = 0;
    	output->bg = 0;
    	output->nd = input->n;
    	return;
    }	
	
    float nepBC = soNepBC(input);  
    float nepBG = soNepBG(input);  
    int ldBC = (input->dc < 8) ? 1 : 2;  
    int ldBG = (input->dg < 5) ? 1 : 2;  
    
    int maxBC = floor(input->n / nepBC);  
    maxBC = (maxBC > input->ld / ldBC) ? input->ld / ldBC : maxBC;
   
    float nepCL = input->n - (maxBC * nepBC);  
    int ldCL = input->ld - (maxBC * ldBC);     

    int maxBG = 0;
    if (nepCL >= nepBG && ldCL >= ldBG) {
        int maxBGtoNep = floor(nepCL / nepBG);
        int maxBGtoLD = ldCL / ldBG;          
        maxBG = (maxBGtoNep < maxBGtoLD) ? maxBGtoNep : maxBGtoLD;
    }

    float nepDu = nepCL - (maxBG * nepBG);

    output->bc = maxBC;
    output->bg = maxBG;
    output->nd = round(nepDu * 1000) / 1000.0;

    if (DEBUG) {
        printf("--> Wind: bc=%d, bg=%d, nd=%.3f\n", output->bc, output->bg, output->nd);
    }
}


void pg_Rain(NL *input, TP *output) {
	if (!serSun() && !kiem_tra_dv(input)) { 
        output->bc = -1;
        output->bg = -1;
        output->nd = input->n;
        return;
    }

	if (input->dc <= 0 && input->dg <= 0) {
    	output->bc = 0;
    	output->bg = 0;
    	output->nd = input->n;
    	return;
    }
		   
    float numbersNepBC = soNepBC(input); 
    float numbersNepBG = soNepBG(input); 

    if (numbersNepBC <= 0 || numbersNepBG <= 0) {
        output->bc = 0;
        output->bg = 0;
        output->nd = input->n;
        return;
    }

    int bestBC = 0, bestBG = 0;          
    float minND = input->n;             
    int minDiff = input->n;
	int bc, bg;
    for (bc = 0; bc <= floor(input->n / numbersNepBC); bc++) {
        float remainingNep = input->n - (bc * numbersNepBC);
        if (remainingNep < 0) continue;

        for (bg = 0; bg <= floor(remainingNep / numbersNepBG); bg++) {
            float nd = remainingNep - (bg * numbersNepBG);
            if (nd < 0) break;

            int requiredLD = soLD(input, bc, bg);
            if (requiredLD > input->ld) continue;

            int diff = abs(bc - bg);

            if (diff < minDiff || (diff == minDiff && nd < minND)) {
                bestBC = bc;
                bestBG = bg;
                minND = nd;
                minDiff = diff;
            }
        }
    }


    while (minND - numbersNepBC >= 0 || minND - numbersNepBG >= 0) {
        int requiredLD = soLD(input, bestBC + 1, bestBG);
        if (minND >= numbersNepBG && requiredLD <= input->ld) {
            bestBG++;
            minND -= numbersNepBG;
        } else if (minND >= numbersNepBC && requiredLD <= input->ld) {
            bestBC++;
            minND -= numbersNepBC;
        } else {
            break;
        }
    }

    output->bc = bestBC;
    output->bg = bestBG;
    output->nd = round(minND * 1000) / 1000.0;

    if (DEBUG)
        printf("--> Rain: bc=%d, bg=%d, nd=%.3f\n", output->bc, output->bg, output->nd);
}

void pg_Sun(NL *input, TP *output) {
    if (!kiem_tra_dv(input)) {
        output->bc = -1;
        output->bg = -1;
        output->nd = input->n;
        isFromSun = 0;
        return;
    }
    
    if (input->dc <= 0 && input->dg <= 0) {
    	output->bc = 0;
    	output->bg = 0;
    	output->nd = input->n;
    	return;
    }	
    
    int original_n = input->n;
    int G = input->dc % 6;
    int H = input->ld % 5;

    int X_table[5][6] = {
        {5, 7, 10, 12, 15, 20},
        {20, 5, 7, 10, 12, 15},
        {15, 20, 5, 7, 10, 12},
        {12, 15, 20, 5, 7, 10},
        {10, 12, 15, 20, 5, 7}
    };

    if (G < 0 || G >= 6 || H < 0 || H >= 5) {
        output->bc = -1;
        output->bg = -1;
        output->nd = original_n;
        isFromSun = 0;
        return;
    }

    int X = X_table[H][G];
    input->n += (input->n * X / 100.0);
    input->ld -= X;
    
    if (input->ld < 0) input->ld = 0;
    
        if (input->ld < 0) { 
        output->bc = -1;
        output->bg = -1;
        output->nd = original_n;
        return;
    }

    int sum = (input->dc + input->dg) % 3;
    if (sum == 0) strcpy(input->w, "Rain");
    else if (sum == 1) strcpy(input->w, "Wind");
    else if (sum == 2) strcpy(input->w, "Cloud");
    else {
        output->bc = -1;
        output->bg = -1;
        output->nd = original_n;
        isFromSun = 0;
        return;
    }
    
    isFromSun = 1;
    if (strcmp(input->w, "Rain") == 0) {
        pg_Rain(input, output);
    } else if (strcmp(input->w, "Wind") == 0) {
        pg_Wind(input, output);
    } else if (strcmp(input->w, "Cloud") == 0) {
        pg_Cloud(input, output);
    }
   
    if (output->bc < 0 || output->bg < 0) {
        output->bc = -1;
        output->bg = -1;
        output->nd = original_n;
        isFromSun = 0;
    }
}

void pg_Fog(NL *input, TP *output) {
    if (!kiem_tra_dv(input)) {
        output->bc = -1;
        output->bg = -1;
        output->nd = input->n;
        return;
    }
	
	if (input->dc <= 0 && input->dg <= 0) {
    	output->bc = 0;
    	output->bg = 0;
    	output->nd = input->n;
    	return;
    }	       

    if (isFibonacci(input->dc) && isFibonacci(input->dg)) {
        input->dc /= 2;
        input->dg /= 2;
    } else {
        input->dc *= 2;
        input->dg *= 2;
    }

    float numbersNepBC = soNepBC(input);
    float numbersNepBG = soNepBG(input);

    if (numbersNepBC < 0 || numbersNepBG < 0) {
        output->bc = -1;
        output->bg = -1;
        output->nd = input->n;
        return;
    }

    int bestBC = 0, bestBG = 0;
    float minTotalNepDu = input->n;
    int minTotalBanh = input->n;
	int bc, bg;
    for (bc = 0; bc <= floor(input->n / numbersNepBC); bc++) {
        float remainingNep = input->n - (bc * numbersNepBC);
        bg = (remainingNep >= numbersNepBG) ? floor(remainingNep / numbersNepBG) : 0;

        int requiredLD = soLD(input, bc, bg);

        if (requiredLD > input->ld) {
            continue;
        }

        float nepDu = remainingNep - (bg * numbersNepBG);
        int totalBanh = bc + bg;

        if (totalBanh < minTotalBanh || (totalBanh == minTotalBanh && nepDu < minTotalNepDu)) {
            bestBC = bc;
            bestBG = bg;
            minTotalBanh = totalBanh;
            minTotalNepDu = nepDu;
        }
    }

    if (minTotalBanh == input->n) {
        output->bc = -1;
        output->bg = -1;
        output->nd = input->n;
    } else {
        output->bc = bestBC;
        output->bg = bestBG;
        output->nd = round(minTotalNepDu * 1000) / 1000.0;
    }

    if (DEBUG) {
        printf("--> Fog: bc=%d bg=%d nd=%.3f\n", output->bc, output->bg, output->nd);
    }
}


void pg_Cloud(NL *input, TP *output) {
    if (!serSun() && !kiem_tra_dv(input)) { 
        output->bc = -1;
        output->bg = -1;
        output->nd = input->n;
        return;
    } 
    
    if (input->dc <= 0 && input->dg <= 0) {
    	output->bc = 0;
    	output->bg = 0;
    	output->nd = input->n;
    	return;
    }	
    
    if (isFriendPair(input->n, input->ld)) {
        output->bc = 0;
        output->bg = 0;
        output->nd = input->n;
        return;
    }

    float numbersNepBC = soNepBC(input);
    float numbersNepBG = soNepBG(input);
    
    int maxBGtoNep = input->n / numbersNepBG; 
    int maxBGtoLD = input->ld / ((input->dg < 5) ? 1 : 2); 
    output->bg = (maxBGtoNep < maxBGtoLD) ? maxBGtoNep : maxBGtoLD;

    float remainingNep = input->n - (output->bg * numbersNepBG);
    
    int maxBCtoNep = remainingNep / numbersNepBC;    
    int maxBCtoLD = (input->ld - output->bg * ((input->dg < 5) ? 1 : 2)) / ((input->dc < 8) ? 1 : 2); 
    output->bc = (maxBCtoNep < maxBCtoLD) ? maxBCtoNep : maxBCtoLD;

    remainingNep -= output->bc * numbersNepBC;
    output->nd = round(remainingNep * 1000) / 1000.0;

    int requiredLD = soLD(input, output->bc, output->bg);
    if (requiredLD > input->ld) {
        output->bc = -1;
        output->bg = -1;
        output->nd = input->n;
        return;
    }

    if (DEBUG) {
        printf("--> Cloud: bc=%d bg=%d nd=%.3f\n", output->bc, output->bg, output->nd);
    }
}



void write_output(const char *filename, const NL *input, const TP *output) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Bug: Not open file %s to write.\n", filename);
        exit(1);
    }
    if (!kiem_tra_dv(input)) {
        fprintf(file, "-1 -1 %d\n", input->n);
    } else {
        fprintf(file, "%d %d %.3f\n", output->bc, output->bg, output->nd);
    }
    fclose(file);
}
void pg_output(const char *inputFile, const char *outputFile) {
    FILE *input = fopen(inputFile, "r");
    FILE *output = fopen(outputFile, "w");

    if (input == NULL || output == NULL) {
        fprintf(stderr, "Can't open input or output file.\n");
        exit(1);
    }

    NL inputDL;
    TP outputDL;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), input)) {
     
        int count = sscanf(buffer, "%d %d %d %d %s", 
                           &inputDL.n, &inputDL.dc, &inputDL.dg, &inputDL.ld, inputDL.w);

        if (count != 5) {
            fprintf(output, "-1 -1 0\n");
            continue;
        }

        toLower(&inputDL);


        if (!kiem_tra_dv(&inputDL)) {
            fprintf(output, "-1 -1 %d\n", inputDL.n);
            continue;
        }

        if (strcmp(inputDL.w, "wind") == 0) {
            pg_Wind(&inputDL, &outputDL);
        } else if (strcmp(inputDL.w, "rain") == 0) {
            pg_Rain(&inputDL, &outputDL);
        } else if (strcmp(inputDL.w, "sun") == 0) {
            pg_Sun(&inputDL, &outputDL);
        } else if (strcmp(inputDL.w, "fog") == 0) {
            pg_Fog(&inputDL, &outputDL);
        } else if (strcmp(inputDL.w, "cloud") == 0) {
            pg_Cloud(&inputDL, &outputDL);
        } else {
            fprintf(output, "-1 -1 %d\n", inputDL.n);
            continue;
        }

        if (outputDL.bc < 0 || outputDL.bg < 0) {
            fprintf(output, "-1 -1 %d\n", inputDL.n);
        } else {
            fprintf(output, "%d %d %.3f\n", outputDL.bc, outputDL.bg, outputDL.nd);
        }
    }

    fclose(input);
    fclose(output);
}


