#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

typedef float proba;
typedef int indice;

#define abs(x) ((x)>0? (x) : (-(x)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// L est le nombre de ligne de la matrice
// C est le nombre de colonne de la matrice
// M est le nombre de valeurs non nulles de la matrice
indice L, C, M;

struct elem *P;
struct elem_normal *P_normal;
proba *x, *y, *f, alpha, *e;
int nbPuits = 50; // est utilisé pour créer un pourcentage de puits
bool puit = false;

// Structure qui représente un sommet du graphe
typedef struct {
    indice s, X, renommage;
} sommet_transformer;

// Structure de données pour stocker une matrice en plein
struct elem {
	sommet_transformer i, j; // indices
	proba val;
};

struct elem_normal {
	indice i, j; // indices
	proba val;
};

struct sommet {
	indice i, j;
};

typedef struct {
    indice *Din;
    indice *Dout;
} Degres;

typedef struct {
	proba val;
	indice pos;
} affichageComparaison;

// ##############################
// # Fonction pour les vecteurs #
// ##############################

// Remplie un vecteur de 0
void assigne_zero(proba *V) {
	indice i;
	for (i=0; i<C; i++)
		V[i] = 0.0;
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

void alloue_memoire_normal() {
	P_normal = malloc(M*sizeof(struct elem));
	if (P_normal==NULL) exit(26);
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

// Fonction qui sépare en deux parties autour du séparateur #.
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

// Permet de dire si un tableau contient une valeur ou non.
int contient(indice *tab, int taille, indice val) {
    for (int i = 0; i < taille; i++) {
        if (tab[i] == val) return 1;
    }
    return 0;
}

// Permet de dire si un tableau contient un sommet transformer ou non.
int contient_sommet(sommet_transformer *tab, int taille, sommet_transformer val) {
    for (int i = 0; i < taille; i++) {
        if (tab[i].s == val.s && tab[i].X == val.X) {
            return i;
        }
    }
    return -1;
}

// Fonction qui permet de lire une matrice de sommets non transformés.
void lire_fichier3(char *nom_fic) {
	FILE *F;
	F = fopen(nom_fic, "r");
	fscanf(F, "%d", &L);
	fscanf(F, "%d", &M);
	C = L;
	alloue_memoire_normal();
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
			P_normal[k].i = indice_ligne;
			fscanf(F, "%d %f", &P_normal[k].j, &P_normal[k].val);
			k++;
		}
	}
	fclose(F);
}

// Fonction qui permet de lire une matrice avec des sommets transformés de type X#1.
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
	fclose(F);
	for (int l = 0; l<C;l++) {
	    printf("sommet %d#%d rename en %d\n", nommage[l].s, nommage[l].X, nommage[l].renommage);
	}
	*taille_nommage = compteur;
	return nommage;
}

// Supprime un certain pourcentage d'acs dans un fichier.
void supprimer_arc(char *nom_fic, char *fic_result, float pourcentage) {
    int nombre = (int) ((pourcentage / 100.0) * C);
    int *supression = malloc(nombre*sizeof(int));
    srand(time(NULL));
    int val;
    for (int i = 0; i<nombre; i++) {
        val = rand() % C;
        while (contient(supression, nombre, val)) {
            val = rand() % C;
        }
        supression[i] = val;
    }
    FILE *F_read = fopen(nom_fic, "r");
	FILE *F_write = fopen(fic_result, "w");
	indice nb_sommet, nb_arcs;
	fscanf(F_read, "%d %d \n", &nb_sommet, &nb_arcs);
	fprintf(F_write, "%d\n%d\n", nb_sommet, nb_arcs);
	char buffer[2048];
	indice arc_restant = nb_arcs;
	for (int ligne = 0; ligne<C; ligne++) {
	    fgets(buffer, sizeof(buffer), F_read);
    	if (contient(supression, nombre, ligne)) {
    	    indice sommet = 0;
    	    sscanf(buffer, "%d %d", &sommet, &nb_arcs);
    	    arc_restant -= nb_arcs;
    	    fprintf(F_write, "%d 0\n", sommet);
    	} else {
    	    fputs(buffer, F_write);
    	}
	}
	fclose(F_read);
	fclose(F_write);
	free(supression);
	F_write = fopen(fic_result, "r+");
    fprintf(F_write, "%d\n%d ", nb_sommet, arc_restant);
    fclose(F_write);
}

// Lit le fichier et calcule les degré entrant et sortant pour chaque sommet.
// Si il y a aucun puit alors on modifie le fichier en enlevant des arcs à des sommets.
Degres lire_degre_fichier(char *nom_fic) {
	FILE *F;
	F = fopen(nom_fic, "r");
    puit = false;
	fscanf(F, "%d", &L);
	fscanf(F, "%d", &M);
	C = L;
	printf("%d\n\n", L);

	indice *Din = calloc(L, sizeof(indice));
	if (Din == NULL) exit(26);
	indice *Dout = calloc(L, sizeof(indice));
	if (Dout == NULL) exit(27);
    
	for (int ligne = 0; ligne < L; ligne++) {
		indice indice_ligne;
		indice sortant;
		fscanf(F, "%d %d", &indice_ligne, &sortant);
		Dout[indice_ligne - 1] = sortant;
		if (sortant == 0) puit = true;
		
		for (int nb_sommet = 0; nb_sommet < Dout[indice_ligne - 1]; nb_sommet++) {
			indice destination;
			fscanf(F, "%d %*f", &destination);
			Din[destination - 1]++;
		}
		indice_ligne++;
	}
	fclose(F);
	Degres degres;
    degres.Din = Din;
    degres.Dout = Dout;
    if (!puit) {
        supprimer_arc(nom_fic, "res.txt", nbPuits);
    }
	return degres;
}


void modif_fichier(char *nom_fic, char *fic_result, Degres degres) {
	
    indice *Din = degres.Din;
    indice *Dout = degres.Dout;
    
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
	fclose(F_read);
	fclose(F_write);
	free(Din);
	free(Dout);
	free(aRemplacer);
	free(sommets);
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
        for (indice i = 0; i < C; i++) {
        y[i] = alpha * y[i] + poid;
    }
}

void mult2_normal(proba *x, proba *y) {
    proba poid;
    poid = ((1.0 - alpha) * (1.0 / C) + alpha * (1.0 / C) * multVecteur(x, f));
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

void mult_normal(proba *x, proba *y, struct elem_normal *P_normal) {
	indice i, j, k;
	proba val;
	struct elem_normal e;
	for (k=0; k<M; k++) {
		e = P_normal[k];
		i = e.i;
		j = e.j;
		val = e.val;
		y[j-1] += x[i-1] * val;
	}

	mult2_normal(x, y);
}


// Norme 1 qui fais la différence
// entre les vecteurs entre 2 itérations
proba norme1 (proba *x, proba *y) {
	indice i;
	proba delta = 0.0;
	for (i=0; i<C; i++) delta += fabs(x[i] - y[i]);
	return delta;
}


int iter_converg(proba *x, proba *y, proba epsilon) {
	proba sum;
	proba delta = 1.0;
	int l = 0;
	while (delta > epsilon) {
		sum = 0.0;
		for (int j = 0; j<C; j++) {
		    if (j % 5000 == 0) {
		        //printf("valeur de l : %d %f \n", l*5000, x[j]);
		        //l += 1;
		    }
		    	
			sum += x[j];
		}
		l++;
		assigne_zero(y);
		mult(x, y, P);
		delta = norme1(x, y);
		recopie(x, y);
	}
	printf("convergence en %d étapes\n", l);
	return l;
}

int iter_converg_normal(proba *x, proba *y, proba epsilon) {
	proba sum;
	proba delta = 1.0;
	int l = 0;
	while (delta > epsilon) {
		sum = 0.0;
		for (int j = 0; j<C; j++) {
		    if (j % 5000 == 0) {
		        //printf("valeur de l : %d %f \n", l*5000, x[j]);
		        //l += 1;
		    }
		    	
			sum += x[j];
		}
		l++;
		assigne_zero(y);
		mult_normal(x, y, P_normal);
		delta = norme1(x, y);
		recopie(x, y);
	}
	printf("convergence en %d étapes\n", l);
	return l;
}

// #################################
// # Ouverture csv + graphique svg #
// #################################

#define MAX_COL_NAME_LEN 64

typedef struct {
    char col1_name[MAX_COL_NAME_LEN];
    char col2_name[MAX_COL_NAME_LEN];
    char col3_name[MAX_COL_NAME_LEN];
    size_t n_rows;
    proba *col1;
    proba *col2;
    proba *col3;
} CSVData;

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

// #####################################
// # Lecture données csv de 3 colonnes #
// #####################################
CSVData *read_csv(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return NULL;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // Lecture de l'en-tête
    nread = getline(&line, &len, f);
    if (nread <= 0) {
        fprintf(stderr, "Fichier vide ou erreur lecture en-tête\n");
        fclose(f);
        free(line);
        return NULL;
    }
    // Supprimer retour chariot
    line[strcspn(line, "\r\n")] = '\0';

    // Découpage de l'en-tête
    char *tok = strtok(line, ",");
    if (!tok) {
		fclose(f);
    	return NULL;
	};
    char hdr1[MAX_COL_NAME_LEN]; strncpy(hdr1, tok, MAX_COL_NAME_LEN);

    tok = strtok(NULL, ",");
    if (!tok) {
		fclose(f);
    	return NULL;
	};
    char hdr2[MAX_COL_NAME_LEN]; strncpy(hdr2, tok, MAX_COL_NAME_LEN);

    tok = strtok(NULL, ",");
    if (!tok) {
		fclose(f);
    	return NULL;
	};
    char hdr3[MAX_COL_NAME_LEN]; strncpy(hdr3, tok, MAX_COL_NAME_LEN);

    // Préparation de la structure
    CSVData *csv = malloc(sizeof *csv);
    if (!csv) {
		fclose(f);
    	return NULL;
	};
    strncpy(csv->col1_name, hdr1, MAX_COL_NAME_LEN);
    strncpy(csv->col2_name, hdr2, MAX_COL_NAME_LEN);
    strncpy(csv->col3_name, hdr3, MAX_COL_NAME_LEN);
    csv->n_rows = 0;

    // Tableaux dynamiques
    size_t cap = 16;
    csv->col1 = malloc(cap * sizeof *csv->col1);
    csv->col2 = malloc(cap * sizeof *csv->col2);
    csv->col3 = malloc(cap * sizeof *csv->col3);
    if (!csv->col1 || !csv->col2 || !csv->col3) {
		free(csv->col1);
		free(csv->col2);
		free(csv->col3);
		free(csv);
		free(line);
		fclose(f);
    	return NULL;
	};

    // Lecture des données
    while ((nread = getline(&line, &len, f)) != -1) {
        if (nread <= 1) continue;
        line[strcspn(line, "\r\n")] = '\0';
        // Découper champs
        char *c1 = strtok(line, ",");
        char *c2 = strtok(NULL, ",");
        char *c3 = strtok(NULL, ",");
        if (!c1 || !c2 || !c3) continue;
        proba v1 = atof(c1);
        int   v2 = atoi(c2);
        int   v3 = atoi(c3);

        // Réallocation si nécessaire
        if (csv->n_rows >= cap) {
            cap *= 2;
            csv->col1 = realloc(csv->col1, cap * sizeof *csv->col1);
            csv->col2 = realloc(csv->col2, cap * sizeof *csv->col2);
            csv->col3 = realloc(csv->col3, cap * sizeof *csv->col3);
            if (!csv->col1 || !csv->col2 || !csv->col3) {
				free(csv->col1);
				free(csv->col2);
				free(csv->col3);
				free(csv);
				free(line);
				fclose(f);
    			return NULL;
			};
        }
        csv->col1[csv->n_rows] = v1;
        csv->col2[csv->n_rows] = v2;
        csv->col3[csv->n_rows] = v3;
        csv->n_rows++;
    }

    free(line);
    fclose(f);
    return csv;
}

// #######################
// # Programme principal #
// #######################

int main(int argc, char *argv[]) {
	if (argc < 2) {
        fprintf(stderr, "Usage : %s <texte>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    float epsilon = 0.000001;

    FILE *fichier_resultats = fopen("convergence.csv", "w");
    if (!fichier_resultats) {
        perror("Erreur lors de l'ouverture de convergence.csv");
        exit(EXIT_FAILURE);
    }

    fprintf(fichier_resultats, "alpha,nb_iterations_backspace,nb_iterations_normal\n");


    for (float a = 0.5; a <= 0.95; a += 0.05) {
        alpha = a;
        printf("==> Test avec alpha = %.2f\n", alpha);

        puit = true;
        Degres degres = lire_degre_fichier(filename);
        if (puit) {
            modif_fichier(filename, "resfinal.txt", degres);
        } else {
            Degres degres = lire_degre_fichier("res.txt");
            modif_fichier("res.txt", "resfinal.txt", degres);
        }

        int taille_nommage;
        sommet_transformer* nommage = lire_fichier4("resfinal.txt", &taille_nommage);
        assigne_zero(y);
        init_x(x);
        int nb_iterations_backspace = iter_converg(x, y, epsilon);

        //regrouper les valeurs des P,X
		indice nbSommet = 0;
		for(int i = 0; i < C; i++) {
			nbSommet = MAX(nbSommet, nommage[i].s);
		}
		affichageComparaison maxBackspace;
		maxBackspace.val = 0.0;
		proba *distrib = calloc(nbSommet, sizeof(proba));
		for(int j = 0; j < C; j++) {
			distrib[nommage[j].s - 1] += x[j];
			if (distrib[nommage[j].s - 1] > maxBackspace.val) {
				maxBackspace.val = distrib[nommage[j].s - 1];
				maxBackspace.pos = nommage[j].s - 1;
			}
		}
        float s1 = 0.0;
        for(int j = 0; j<nbSommet; j++) {
            printf("val de x pour backspace : %f\n", distrib[j]);
            s1 += distrib[j];
        }
        printf("somme backspace %f\n", s1);
        
        // Libération mémoire
        //free(nommage);
        free(P);
        free(x);
        //free(y);
        free(f);
        
		if (puit) {
            lire_fichier3(filename);
        } else {
            lire_fichier3("res.txt");
        }
        assigne_zero(y);
        init_x(x);
        int nb_iterations_normal = iter_converg_normal(x, y, epsilon);
        float s2 = 0.0;
        for(int j = 0; j<C; j++) {
            printf("val de x pour normale : %f\n", x[j]);
            s2 += x[j];
        }
        printf("somme normale %f\n", s2);
		printf("La plus grande valeur pour backspace est %f pour le sommet %d alors que le sommet pour PageRank est de %f\n", maxBackspace.val, maxBackspace.pos, x[maxBackspace.pos]);
        free(P_normal);
        free(x);
        free(y);
        free(f);
        fprintf(fichier_resultats, "%.2f,%d,%d\n", alpha, nb_iterations_backspace, nb_iterations_normal);
    }

    fclose(fichier_resultats);
    printf("Fichier convergence.csv généré.\n");

	const char *filenamecsv = "convergence.csv";
	CSVData *csv = read_csv(filenamecsv);
	if (csv == NULL) {
		printf("Problème à la lecture du fichier %s", filenamecsv);
	} else {
		svg(csv->col1, csv->col2, "Backspace", csv->col1, csv->col3, "Normal", csv->n_rows, "Alpha", "Nb it pour convergence");
	}
    
    return 0;
}
