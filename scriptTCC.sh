clear
echo "--> Compilando -"
gcc -fopenmp structures.h utilities.h searches.h grasp.h main.c  -I/usr/include/igraph -L/usr/local/lib -ligraph -o alg

echo "Grafo teste"
./alg "teste"

echo "Fim"
