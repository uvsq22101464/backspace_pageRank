#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>
#include <math.h>  // Pour fabs()
#include <sys/time.h>
#include <string.h>

typedef float proba;
typedef int indice;

#define abs(x) ((x)>0? (x) : (-(x)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// L est le nombre de ligne de la matrice
// C est le nombre de colonne de la matrice
// M est le nombre de valeurs non nulles de la matrice
indice L, C, M;

struct elem *P;
proba *x, *y, *f, alpha, *e;

// Structure qui représente un sommet du graphe
typedef struct {
    indice s, X, renommage;
} sommet_transformer;

// Structure de données pour stocker une matrice en plein
struct elem {
	sommet_transformer i, j; // La taille
	proba val; // Les valeurs
};


// ##############################
// # Fonction pour les vecteurs #
// ##############################

// Remplie un vecteur de 0
void assigne_zero(proba *V) {
	indice i;
	for (i=0; i<C; i++)
		V[i] = 0.0;
}

void assigne_un(proba *V) {
    indice i;
    for (i=0; i<C; i++) {
        V[i] = 1.0;
    }
}

void assigne_proba(proba *V, proba e) {
    indice i;
    for (i=0; i<C; i++) {
        V[i] = e;
    }
}

// Initialise le x avec une proba
// équivalente dans chaque case
void init_x(proba *V) {
	indice i;
	for (i=0; i<C; i++)
		V[i] = 1.0/(float)C;
}

// Recopie un vecteur dans un autre
void recopie(proba *x, proba *y) {
	indice i;
	for (i=0; i<C; i++) 
		x[i] = y[i];
}

// ##################
// # Initialisation #
// ##################

// Alloue de la mémoire avec malloc 
void alloue_memoire() {
	P = malloc(M*sizeof(struct elem));
	if (P==NULL) exit(26);
	x = malloc(C*sizeof(proba));
	if (x==NULL) exit(22);
	init_x(x);
	y = malloc(C*sizeof(proba));
	if (y==NULL) exit(23);
	assigne_zero(y);
	f = malloc(C*sizeof(proba));
	if (f==NULL) exit(24);
	assigne_zero(f);
	e = malloc(C*sizeof(proba));
	if (e==NULL) exit(25);
}

void lire_transformation(char* sommet_temp, char* partie1, char* partie2) {
    memset(partie1, 0, sizeof(partie1));
    memset(partie2, 0, sizeof(partie2));
    bool avantDelimiteur = true;
    int i = 0, j = 0;
    for (int longueur = 0; sommet_temp[longueur] != '\0'; longueur++) {
        char c = sommet_temp[longueur];
        if (c == '#') {
            avantDelimiteur = false;
            continue;
        }
        if (avantDelimiteur) {
            partie1[i] = c;
            i++;
        } else {
            partie2[j] = c;
            j++;
        }
    }
}

int contient(indice *tab, int taille, indice val) {
    for (int i = 0; i < taille; i++) {
        if (tab[i] == val) return 1;
    }
    return 0;
}

int contient_sommet(sommet_transformer *tab, int taille, sommet_transformer val) {
    for (int i = 0; i < taille; i++) {
        if (tab[i].s == val.s && tab[i].X == val.X) {
            return i;
        }
    }
    return -1;
}

sommet_transformer* lire_fichier4(char *nom_fic, int *taille_nommage) {
	FILE *F;
	F = fopen(nom_fic, "r");
	fscanf(F, "%d", &L);
	fscanf(F, "%d", &M);
	C = L;
	alloue_memoire();
	sommet_transformer *nommage = malloc(M*sizeof(sommet_transformer));
	indice compteur = 0;
	indice k = 0;
	for (int ligne = 0; ligne < L; ligne++) {
		int Dout;
		sommet_transformer indice_ligne;
		char sommet_temporaire[1+2*C];
		char partie1[C], partie2[C];
		fscanf(F, "%s %d", sommet_temporaire, &Dout);
		lire_transformation(sommet_temporaire, partie1, partie2);
		//printf("debug partie 1 : %s\npartie 2 : %s\n", partie1, partie2);
		indice_ligne.s = atoi(partie1);
		indice_ligne.X = atoi(partie2);
		
		int position = contient_sommet(nommage, compteur, indice_ligne);
		if (position != -1) {
		    indice_ligne.renommage = position;
		} else {
		    indice_ligne.renommage = compteur;
		    nommage[compteur] = indice_ligne;    
		    compteur++;
		}
        
        
		
		for (int nb_sommet = 0; nb_sommet < Dout; nb_sommet++) {
			P[k].i = indice_ligne;
			
			char sommet_temp[1+2*C];
			char part1[C], part2[C];
			bool avantDelimiteur = true;
			int i = 0, j = 0;
			//fscanf(F, "%s %f", &P[k].j, &P[k].val);
			fscanf(F, "%s %f", sommet_temp, &P[k].val);
            lire_transformation(sommet_temp, part1, part2);
            P[k].j.s = atoi(part1);
            P[k].j.X = atoi(part2);
            
            
            int position = contient_sommet(nommage, compteur, P[k].j);
    		if (position != -1) {
    		    P[k].j.renommage = position;
    		} else {
    		    P[k].j.renommage = compteur;
    		    nommage[compteur] = P[k].j;    
    		    compteur++;
    		}
            
			k++;
		}
	}
	for (int l = 0; l<C;l++) {
	    printf("sommet %d#%d rename en %d\n", nommage[l].s, nommage[l].X, nommage[l].renommage);
	}
	*taille_nommage = compteur;
	return nommage;
}



typedef struct {
    indice *Din;
    indice *Dout;
} Degres;

Degres lire_degre_fichier(char *nom_fic) {
	FILE *F;
	F = fopen(nom_fic, "r");

	fscanf(F, "%d", &L);
	fscanf(F, "%d", &M);
	C = L;
	printf("%d\n\n", L);

	indice *Din = calloc(L, sizeof(indice));
	if (Din == NULL) exit(26);
	indice *Dout = calloc(L, sizeof(indice));
	if (Dout == NULL) exit(27);
    
	//indice k = 0;
	for (int ligne = 0; ligne < L; ligne++) {
		indice indice_ligne;
		indice sortant;
		fscanf(F, "%d %d", &indice_ligne, &sortant);
		Dout[indice_ligne - 1] = sortant;
        //printf("%d \n", Dout[indice_ligne - 1]);
        
		
		for (int nb_sommet = 0; nb_sommet < Dout[indice_ligne - 1]; nb_sommet++) {
			indice destination;
			fscanf(F, "%d %*f", &destination);
			Din[destination - 1]++;

			//P[k].i = indice_ligne;
			//&P[k].j = destination;
			//k++;
		}
		indice_ligne++;
	}
	fclose(F);
	Degres degres;
    degres.Din = Din;
    degres.Dout = Dout;
	return degres;
}

struct sommet {
	indice i, j;
};


void modif_fichier(char *nom_fic, char *fic_result) {
	Degres degres = lire_degre_fichier(nom_fic);
    indice *Din = degres.Din;
    indice *Dout = degres.Dout;
    
    for (int n = 0; n<C; n++) {
        printf("%d, ", Din[n]);
    }
    printf("\n");
    for (int n = 0; n<C; n++) {
        printf("%d, ", Dout[n]);
    }
    printf("\n");
    
	indice *aRemplacer = malloc(L * sizeof(indice));
	indice k = 0;
	indice sommets_supp = 0;
	for (int i = 0; i < L; i++) {
		if (Dout[i] == 0) {
			aRemplacer[k] = i;
			k++;
			sommets_supp += Din[i];
		}
	}
	
	
	printf("sommet supp = %d\n", sommets_supp - k);
	FILE *F_read = fopen(nom_fic, "r");
	FILE *F_write = fopen(fic_result, "w");
	indice nb_sommet, nb_arcs;
	fscanf(F_read, "%d %d", &nb_sommet, &nb_arcs);
	fprintf(F_write, "%d\n%d\n", nb_sommet + sommets_supp - k, nb_arcs + sommets_supp);
	struct sommet *sommets = malloc((sommets_supp) * sizeof(struct sommet));
	indice l = 0;
	for (indice ligne = 0; ligne < nb_sommet; ligne++) {
		indice origne, fils;
		fscanf(F_read, "%d %d", &origne, &fils);
		if (fils != 0) {
			fprintf(F_write, "%d %d ", origne, fils);
			for (int i = 0; i < fils; i++) {
				indice destination;
				proba probabilite;
				fscanf(F_read, "%d %f", &destination, &probabilite);
				if (contient(aRemplacer, k, destination - 1)) {
					fprintf(F_write, "%d#%d %f ", destination, origne, probabilite);
					sommets[l].j = origne;
					sommets[l].i = destination;
					l++;
				} else {
					fprintf(F_write, "%d %f ", destination, probabilite);
				}
			}
			fprintf(F_write, "\n");
		}
		
	}
	// rajouter les acs inverses
	for (indice i = 0; i < sommets_supp; i++) {
		fprintf(F_write, "%d#%d 1 %d %f\n", sommets[i].i, sommets[i].j, sommets[i].j, 1.0);
	}
	//fclose(F_read);
	//fclose(F_write);
	//free(Din);
	//free(Dout);
	//free(aRemplacer);
	//free(sommets);fr
}


// #############
// # Opération #
// #############

proba multVecteur(proba *x, proba *y) {
    proba res = 0.0;
    for (indice i = 0; i < C; i++) {
        res += x[i] * y[i];
    }
    return res;
}

void mult2(proba *x, proba *y) {
    proba poid;
    poid = ((1.0 - alpha) * (1.0 / C));
    
    //assigne_proba(e, poid);
    for (indice i = 0; i < C; i++) {
        y[i] = alpha * y[i] + poid;
    }
}

// Multiplication d'un vecteur avec une matrice creuse
void mult(proba *x, proba *y, struct elem *P) {
	indice i, j, k;
	proba val;
	struct elem e;
	for (k=0; k<M; k++) {
		e = P[k];
		i = e.i.renommage;
		j = e.j.renommage;
		val = e.val;
		y[j] += x[i] * val;

	}
	mult2(x, y);
}


// Norme 1 qui fais la différence
// entre les vecteurs entre 2 itérations
proba norme1 (proba *x, proba *y) {
	indice i;
	proba delta = 0.0;
	for (i=0; i<C; i++) delta += fabs(x[i] - y[i]);
	return delta;
}


void iter_converg(proba *x, proba *y, proba epsilon) {
	proba sum;
	proba delta = 1.0;
	int l = 0;
	while (delta > epsilon) {
		sum = 0.0;
		for (int j = 0; j<C; j++) {
		    if (j % 5000 == 0) {
		        //printf("valeur de l : %d %f \n", l*5000, x[j]);
		        l += 1;
		    }
			
			sum += x[j];
		}
		//printf(" (%g)\n", sum);
		assigne_zero(y);
		mult(x, y, P);
		delta = norme1(x, y);
		//printf("%f\n", delta);
		recopie(x, y);
	}
}

void aff (struct elem *P) {
	indice i;
	printf("%d %d %d\n", L, C, M);
	for (i=0 ; i<M ; i++)
		printf("%d#%d %d#%d %g\n",P[i].i.s, P[i].i.X, P[i].j.s, P[i].j.X, P[i].val);
		//printf("%d %d %g\n",P[i].i.s, P[i].j.s, P[i].val);
}


// #######################
// # Programme principal #
// #######################

void normaliser(proba* v, int taille) {
    float somme = 0.0f;
    for (int i = 0; i < taille; i++) {
        somme += v[i];
    }
    if (somme == 0.0f) return;  // Évite division par zéro
    for (int i = 0; i < taille; i++) {
        v[i] /= somme;
    }
}


// Programme principal
int main() {
    alpha = 0.85;
	//modif_fichier("G8.txt", "res.txt");
	int taille_nommage;
	sommet_transformer* nommage = lire_fichier4("res.txt", &taille_nommage);
	//aff(P);

	iter_converg(x, y, 0.000001);
	float s = 0.0;
    for(int j = 0; j<C; j++) {
        printf("val de x : %f\n", x[j]);
        s += x[j];
    }
    printf("%d valeurs, somme de x : %f\n", C, s);
    
    
    //regrouper les valeurs...
    
    indice nbSommet = 0;
    for(int i = 0; i < C; i++) {
        nbSommet = MAX(nbSommet, nommage[i].s);
    }
    proba *distrib = calloc(nbSommet, sizeof(proba));
    for(int j = 0; j < C; j++) {
        distrib[nommage[j].s - 1] += x[j];
    }
    //normaliser(distrib, nbSommet);
    float s2 = 0.0;
    for(int j = 0; j<nbSommet; j++) {
        printf("val de distrib : %f\n", distrib[j]);
        s2 += distrib[j];
    }
    printf("%d valeurs, somme de x : %f\n", nbSommet, s2);
	exit(0);
}
