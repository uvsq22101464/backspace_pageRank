#include "stdio.h"
#include "stdlib.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

typedef float proba;
typedef int indice;

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

// ########
// # Main #
// ########
int main() {
    const char *filename = "convergence.csv";
    CSVData *csv = read_csv(filename);
	if (csv == NULL) {
		printf("Problème à la lecture du fichier %d", filename);
	} else {
		svg(csv->col1, csv->col2, csv->col2_name, csv->col1, csv->col3, csv->col3_name, csv->n_rows, csv->col1_name, csv->col2_name);
	}
	exit(0);
}