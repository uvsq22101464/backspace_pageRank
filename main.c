#include "stdio.h"
#include "stdlib.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

typedef float proba;
typedef int indice;

#define abs(x) ((x)>0? (x) : (-(x)))

// L est le nombre de ligne de la matrice
// C est le nombre de colonne de la matrice
// M est le nombre de valeurs non nulles de la matrice
indice L, C, M;

struct elem *P;
proba *x, *y, *f, alpha, *e;

// Structure de données pour stocker une matrice en plein
struct elem {
	indice i, j;     // La taille
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
	assigne_un(f);
	e = malloc(C*sizeof(proba));
	if (e==NULL) exit(25);
}

// Lit le fichier "nom_fic" qui contient une matrice creuse,
// alloue la mémoire nécessaire et stocke la matrice
// dans un "struct elem"
void lire_fichier(char *nom_fic) {
	indice i;
	FILE *F;
	F = fopen(nom_fic,"r");
	fscanf(F,"%d %d %d", &(L), &(C), &(M));
	alloue_memoire();
	for (i=0 ; i<M ; i++) {
		fscanf(F,"%d %d %f", &(P[i].i), &(P[i].j), &(P[i].val));
		f[P[i].i] = 0.0;
	}
	fclose(F);
}

void lire_fichier3(char *nom_fic) {
	FILE *F;
	F = fopen(nom_fic, "r");
	fscanf(F, "%d", &L);
	fscanf(F, "%d", &M);
	C = L;
	alloue_memoire();
	indice k = 0;
	for (int ligne = 0; ligne < L; ligne++) {
		int indice_ligne, Dout;
		fscanf(F, "%d %d", &indice_ligne, &Dout);
		if (Dout == 0) {
			f[indice_ligne - 1] = 1.0;
		} else {
			f[indice_ligne - 1] = 0.0;
		}
		
		for (int nb_sommet = 0; nb_sommet < Dout; nb_sommet++) {
			P[k].i = indice_ligne;
			fscanf(F, "%d %f", &P[k].j, &P[k].val);
			k++;
		}
	}
	fclose(F);
}

int contient(indice *tab, int taille, indice val) {
    for (int i = 0; i < taille; i++) {
        if (tab[i] == val) return 1;
    }
    return 0;
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
        printf("%d \n", Dout[indice_ligne - 1]);
        
		
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
	indice *aRemplacer = malloc(L * sizeof(indice));
	indice k = 0;
	indice sommets_supp = 0;
	for (int i = 0; i < L; i++) {
		if (Dout[i] == 0) {
			// remplace dans le fichier les valeurs i par (i, la valeur au début de la ligne actuelle)
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
					fprintf(F_write, "(%d, %d) %f ", destination, origne, probabilite);
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
		fprintf(F_write, "(%d, %d) 1 %d %f\n", sommets[i].i, sommets[i].j, sommets[i].j, 1.0);
	}
	//fclose(F_read);
	//fclose(F_write);
	//free(Din);
	//free(Dout);
	//free(aRemplacer);
	//free(sommets);fr
}

// fichier creux sans les probas
void lire_fichier2(char *nom_fic) {
    FILE *F = fopen(nom_fic, "r");
    if (!F) {
        perror("Erreur ouverture fichier");
        exit(EXIT_FAILURE);
    }

    /* Lecture de la première ligne */
    fscanf(F, "%d %d %d", &L, &C, &M);

    alloue_memoire();               /* alloue P[M] et f[L] */

    /* On stocke temporairement les degrés sortants */
    int *deg = calloc(L, sizeof(int));
    if (!deg) {
        fprintf(stderr, "Erreur allocation deg\n");
        exit(EXIT_FAILURE);
    }

    /* Lecture de tous les arcs (i, j) */
    for (indice k = 0; k < M; k++) {
        fscanf(F, "%d %d", &P[k].i, &P[k].j);
        deg[P[k].i]++;             /* incrémente le degré de la ligne i */
    }
    fclose(F);

    /* Calcul des probabilités et initialisation de f */
    for (indice k = 0; k < M; k++) {
        int row = P[k].i;
        P[k].val = 1.0f / deg[row];
        f[row]   = 0.0f;           /* si f sert à accumuler ultérieurement */
    }

    //free(deg);
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
    poid = ((1.0 - alpha) * (1.0 / C) + alpha * (1.0 / C) * multVecteur(x, f));
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
		i = e.i;
		j = e.j;
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

// Affiche la matrice stockée dans le "struct matrice" "T"
void aff (struct elem *P) {
	indice i;
	printf("%d %d %d\n", L, C, M);
	for (i=0 ; i<M ; i++)
		printf("%d %d %g\n",P[i].i, P[i].j, P[i].val);
}

void iter_converg(proba *x, proba *y, proba epsilon) {
	proba sum;
	proba delta = 1.0;
	int l = 0;
	while (delta > epsilon) {
		sum = 0.0;
		for (int j = 0; j<C; j++) {
		    if (j % 5000 == 0) {
		        printf("valeur de l : %d %f \n", l*5000, x[j]);
		        l += 1;
		    }
			
			sum += x[j];
		}
		//printf(" (%g)\n", sum);
		assigne_zero(y);
		mult(x, y, P);
		delta = norme1(x, y);
		recopie(x, y);
	}
}

// #########################################
// # Fonction de création de graphique SVG #
// #########################################
int svg(proba *x1, proba *y1, const char *label1, proba *x2, proba *y2, const char *label2, size_t n, const char *xlabel, const char *ylabel) {
	// Compteur pour le nom du fichier
	static int graph_counter = 0;
    graph_counter++;
	
	const int width   = 500;
    const int height  = 350;
    const int margin  = 50;
	proba xmin = x1[0];
	proba xmax = x1[0];
	proba ymin = y1[0];
	proba ymax = y1[0];

	for (size_t i = 0; i<n; i++) {
		if(x1[i] < xmin) 
			xmin = x1[i];
		if(x1[i] > xmax) 
			xmax = x1[i];
		if(y1[i] < ymin) 
			ymin = y1[i];
		if(y1[i] > ymax) 
			ymax = y1[i];
		if(x2[i] < xmin) 
			xmin = x2[i];
		if(x2[i] > xmax) 
			xmax = x2[i];
		if(y2[i] < ymin) 
			ymin = y2[i];
		if(y2[i] > ymax) 
			ymax = y2[i];
	}

	// Créer le nom du fichier SVG avec timestamp
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    char filename[64];
    snprintf(filename, sizeof(filename),
             "graph_%04d%02d%02d_%02d%02d%02d_%d_%d.svg",
             tm.tm_year + 1900,    // année
             tm.tm_mon  + 1,       // mois
             tm.tm_mday,           // jour
             tm.tm_hour,           // heure
             tm.tm_min,            // minute
             tm.tm_sec,            // seconde
			 getpid(),         	   // PID du processus
             graph_counter);   

	// Ouvrir le fichier SVG en écriture
	FILE *f = fopen(filename, "w");
    if (!f) { 
		perror("graph.svg"); 
		return 0; 
	}

	// Début du fichier SVG en XML
	fprintf(f,
        "<?xml version=\"1.0\"?>\n"
        "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\">\n"
        "  <!-- Axe X -->\n"
        "  <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"black\"/>\n"
        "  <!-- Axe Y -->\n"
        "  <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"black\"/>\n",
        width, height,
        margin, height-margin, width-margin, height-margin,
        margin, height-margin, margin, margin);

	// Graduations des axes
	proba stepx = (xmax - xmin) / 6.0;
    proba stepy = (ymax - ymin) / 6.0;

    for (int i = 0; i <= 6; i++) {
		proba xv = xmin + i * stepx;
        proba X = margin + (xv - xmin) * (width - 2*margin) / (xmax - xmin);
        fprintf(f,
          "  <line x1=\"%g\" y1=\"%d\" x2=\"%g\" y2=\"%d\" stroke=\"black\"/>\n"
          "  <text x=\"%g\" y=\"%d\" font-size=\"10\" text-anchor=\"middle\">%.1f</text>\n",
          X, height-margin,
          X, height-margin+6,
          X, height-margin+20,
          xv);
    }

    for (int i = 0; i <= 6; i++) {
        double yv = ymin + i * stepy;
        double Y = height - margin - (yv - ymin) * (height - 2*margin) / (ymax - ymin);
        fprintf(f,
          "  <line x1=\"%d\" y1=\"%g\" x2=\"%d\" y2=\"%g\" stroke=\"black\"/>\n"
          "  <text x=\"%d\" y=\"%g\" font-size=\"10\" text-anchor=\"end\" dominant-baseline=\"middle\">%.1f</text>\n",
          margin-6, Y,
          margin,   Y,
          margin-8, Y,
          yv);
    }

	// Nommer les axes
    fprintf(f,
      "  <text x=\"%d\" y=\"%d\" font-size=\"14\" text-anchor=\"middle\">%s</text>\n",
      width/2,
      height - margin/3,
      xlabel
    );

    fprintf(f,
      "  <text x=\"%d\" y=\"%d\" font-size=\"14\" text-anchor=\"middle\"\n"
      "        transform=\"rotate(-90 %d %d)\">%s</text>\n",
      margin/3,
      height/2,
      margin/3, height/2,
      ylabel
	  
    );

	// Tracer première courbe
	fprintf(f, "  <polyline fill=\"none\" stroke=\"red\" stroke-width=\"2\" points=\"");
	for (size_t i = 0; i<n; i++) {
		int x = margin + (x1[i] - xmin) * (width - 2*margin) / (xmax - xmin);
		int y = height - margin - (y1[i] - ymin) * (height - 2*margin) / (ymax - ymin);
		fprintf(f, "%d,%d ", x, y);
	}
	fprintf(f, "\"/>\n");

	// Tracer deuxième courbe
	fprintf(f, "  <polyline fill=\"none\" stroke=\"blue\" stroke-width=\"2\" points=\"");
	for (size_t i = 0; i<n; i++) {
		int x = margin + (x2[i] - xmin) * (width - 2*margin) / (xmax - xmin);
		int y = height - margin - (y2[i] - ymin) * (height - 2*margin) / (ymax - ymin);
		fprintf(f, "%d,%d ", x, y);
	}
	fprintf(f, "\"/>\n");

	// Légende
    int lx = margin + 10;
    int ly = margin + 10;
    fprintf(f,
      "  <!-- Cadre de légende -->\n"
      "  <rect x=\"%d\" y=\"%d\" width=\"120\" height=\"50\" fill=\"white\" stroke=\"black\"/>\n"
      "  <!-- %s -->\n"
      "  <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"red\" stroke-width=\"2\"/>\n"
      "  <text x=\"%d\" y=\"%d\" font-size=\"12\">%s</text>\n"
      "  <!-- %s -->\n"
      "  <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"blue\" stroke-width=\"2\"/>\n"
      "  <text x=\"%d\" y=\"%d\" font-size=\"12\">%s</text>\n",
      lx, ly,
	  label1,
      lx+10, ly+15, lx+30, ly+15, lx+35, ly+19,
	  label1, label2,
      lx+10, ly+35, lx+30, ly+35, lx+35, ly+39,
	  label2);

	// Fin du SVG
    fprintf(f, "</svg>\n");
    fclose(f);

	printf("graph.svg généré avec %zu points par courbe.\n", n);
	return 1;
}

// #######################
// # Programme principal #
// #######################

// Programme principal
int main() {
    alpha = 0.85;
	modif_fichier("courtois.txt", "res.txt");
	// aff(P);
	//iter_converg(x, y, 0.00001);

	exit(0);
}
