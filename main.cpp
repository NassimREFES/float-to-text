//------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define DIX 10
#define CENT (DIX*DIX)
#define MILLE (CENT*DIX)
#define MILLION (MILLE*MILLE)

//------------------------------------------------------------------------

inline void error(const char* msg)
{
	printf("%s\n",msg);
	exit(EXIT_FAILURE);
}

//------------------------------------------------------------------------

const char* unite[10] = { "zero", "un", "deux", "trois", "quatre",
			"cinq", "six", "sept", "huit", "neuf"
		        };

const char* entre_11_19[9]  = { "onze", "douze", "treize", "quatorze", 	"quinze",
			        "seize", "dix-sept", "dix-huit", "dix-neuf"
			      };

const char* dizaines[9] = { "dix", "vingt", "trente", "quarante", "cinquante",
			  "soixante", "soixante-dix", "quatre-vingts",
		          "quatre-vingt-dix"
		    	  };

//------------------------------------------------------------------------

char result[100]="";
char resultDiv[100]="";
char resultMille[100]="";
char resultMillion[200]="";
char resultGlobal[200]="";

//------------------------------------------------------------------------

const char* cUnit(int* x)
{
	int ind = *x;
	*x-=*x;
	return unite[ind]; // 1 2 3 4 5 6 ...
}

//------------------------------------------------------------------------

const char* cDizaines(int* x)
{
	int ind = *x;
	if (*x%DIX==0 || *x>19) { // 10 20 30 40 50 ... || x>19
		if ((70<*x && *x<80) || (90<*x && *x<100)) { // 70<x<80 || 90<x<100
			*x -= *x-10-*x%DIX;
			strcpy(result, dizaines[ind/DIX-2]);

		}
		else {
			*x-=DIX*(*x/DIX);
			strcpy(result, dizaines[ind/DIX-1]);
		}
	}
	else { // 11 12 13 14 15 16 ...
		*x-=DIX+*x%DIX;
		strcpy(result, entre_11_19[ind%DIX-1]);
		return result;
	}

	// d'apré les nombres en francais
	if (80<ind && ind<100 && ind!=90)
		strcat(result, "\b-"); // enlever le 's' de 80
	else if (ind%10==1)
		strcat(result, " et "); // pr 21 31 41 51 61 ...
	else if (ind%10!=0) strcat(result, "-"); // les autres

	return result;
}

//------------------------------------------------------------------------

const char* cCent(int* x)
{
	int ind = *x;
	*x-=CENT*(*x/CENT);

	if (ind/CENT>1) strcpy(result, unite[ind/CENT]);
	else strcpy(result, "");

	strcat(result, " cent ");

	return result;
}

//------------------------------------------------------------------------

/*
 * gere les [xxx]
*/

const char* divs(int* dis)
{
	strcpy(resultDiv, ""); // la vidé avan lutilisation
	while (*dis>0) {
		if (*dis/CENT != 0) {
			strcat(resultDiv, cCent(dis));
		}
		else if (*dis/DIX != 0) {
			strcat(resultDiv, cDizaines(dis));
		}
		else {
			strcat(resultDiv, cUnit(dis));
		}
	}
	return resultDiv;
}

//------------------------------------------------------------------------

/* [xxx]xxx
     |   |
     |   +--- se fais dans main
     +--- se fais dans cMille + " mille ";
*/

const char* cMille(int* x)
{
	int dis = *x/MILLE;
	if (dis>0) { // calcule les millier
		if (dis!=1) // pour ecrir mille au lieu de un mille
			strcpy(resultMille, divs(&dis));
		strcat(resultMille, " mille ");
		*x %= MILLE;
	}

	return resultMille;
}

//------------------------------------------------------------------------

/*    [xxx][xxx]xxx
	 |   |   |
	 |   |   +--- se fais dans la fonction de conversion (dans la boucle)
	 |   +--- se fais dans cMille + " mille ";
	 +--- se fais dans cMillion + " million ";
*/

const char* cMillion(int* x)
{
	int dis = *x/MILLION;
	if (dis>0) { // calcule les millions
		strcpy(resultMillion, divs(&dis));
		strcat(resultMillion, " million ");
		*x %= MILLION;
	}

	strcat(resultMillion, cMille(x));
	return resultMillion;
}

//------------------------------------------------------------------------

const char* integerToString(int x)
// convertire un entier en chaine
{
	strcpy(resultGlobal, ""); // le vidé avan lutiliser
	if (x==0) strcat(resultGlobal, "zero ");
	while (x>0) {
		if (x/MILLION != 0) {
			strcat(resultGlobal, cMillion(&x));
		}
		else if (x/MILLE != 0) {
			strcat(resultGlobal, cMille(&x));
		}
		else if (x/CENT != 0) {
			strcat(resultGlobal, cCent(&x));
		}
		else if (x/DIX != 0) {
			strcat(resultGlobal, cDizaines(&x));
		}
		else {
			strcat(resultGlobal, cUnit(&x));
		}
	}
	return resultGlobal;
}

//------------------------------------------------------------------------

const char* doubleToStringAvecDevise(double x, const char* deviseUtiliser)
// avec devise / represente largent
{
	int avant_virgule = (int)x;
	//0.001 pr avoir une conversion juste
	int apre_virgule = (x-avant_virgule+0.001)*CENT;

	integerToString(avant_virgule);
	strcat(resultGlobal, deviseUtiliser);

	strcat(resultGlobal, " virgule ");

	if (apre_virgule<10) strcat(resultGlobal, "zero "); // 0.01 0.02 0.03 ...
	strcat(resultGlobal, divs(&apre_virgule));
	strcat(resultGlobal, " cts "); // centimes

	return resultGlobal;
}

//------------------------------------------------------------------------

const char* doubleToString(double x)
// convertie un nombre en lettre
{
	int avant_virgule = (int)x;
	//0.001 pr avoir une conversion juste
	int apre_virgule = (x-avant_virgule+0.001)*CENT;

	if (apre_virgule>99) apre_virgule/=CENT;

	integerToString(avant_virgule);
	strcat(resultGlobal, " virgule ");

	if (apre_virgule<10 && apre_virgule!=0) strcat(resultGlobal, "zero ");
	strcat(resultGlobal, divs(&apre_virgule));

	return resultGlobal;
}

//------------------------------------------------------------------------

bool valid_number(double d)
{
    static const double MAX = 999999999.99;
    static const double MIN = 0;
    return d>=MIN && d <=MAX;
}

int main()
{
	int i;

	double number;
	printf("Entrer un nombre < 999999999.99\n");
	scanf("%lf", &number);

    if (valid_number(number))
        printf("%s\n\n", doubleToStringAvecDevise(number, " dinars "));
    else
        error("invalid number");

	return 0;
}

//------------------------------------------------------------------------
