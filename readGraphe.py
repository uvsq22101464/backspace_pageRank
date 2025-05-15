import matplotlib.pyplot as plt
import csv

alphas = []
nb_iterations_backspace = []
nb_iterations_normal = []

with open("convergence.csv", "r") as f:
    reader = csv.DictReader(f)
    for row in reader:
        alphas.append(float(row['alpha']))
        nb_iterations_backspace.append(int(row['nb_iterations_backspace']))
        nb_iterations_normal.append(int(row['nb_iterations_normal']))

plt.plot(alphas, nb_iterations_backspace, marker='o', label='Backspace')
plt.plot(alphas, nb_iterations_normal, marker='x', label='Normal')
plt.xlabel('Valeur de alpha')
plt.ylabel("Nombre d'itérations pour convergence")
plt.title("Convergence du PageRank en fonction de alpha")
plt.grid(True)
plt.legend()
plt.show()
