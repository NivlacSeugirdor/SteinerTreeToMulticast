clear
clear
clear
clear
clear
echo "---> Compilando ---"
gc -fopenmp structures.h utilities.h searches.h grasp.h main.c -I/usr/include/igraph -L/usr/local/lib -ligraph -o TCC2



echo "Grafo teste"

./TCC2 "teste" "1" "0.000000"
./TCC2 "teste" "1" "0.100000"
./TCC2 "teste" "1" "0.200000"
./TCC2 "teste" "1" "0.300000"
./TCC2 "teste" "1" "0.400000"
./TCC2 "teste" "1" "0.500000"
./TCC2 "teste" "1" "0.600000"
./TCC2 "teste" "1" "0.700000"
./TCC2 "teste" "1" "0.800000"
./TCC2 "teste" "1" "0.900000"
./TCC2 "teste" "1" "1.000000"
./TCC2 "teste" "2" "0.000000"
./TCC2 "teste" "2" "0.100000"
./TCC2 "teste" "2" "0.200000"
./TCC2 "teste" "2" "0.300000"
./TCC2 "teste" "2" "0.400000"
./TCC2 "teste" "2" "0.500000"
./TCC2 "teste" "2" "0.600000"
./TCC2 "teste" "2" "0.700000"
./TCC2 "teste" "2" "0.800000"
./TCC2 "teste" "2" "0.900000"
./TCC2 "teste" "2" "1.000000"
./TCC2 "teste" "4" "0.000000"
./TCC2 "teste" "4" "0.100000"
./TCC2 "teste" "4" "0.200000"
./TCC2 "teste" "4" "0.300000"
./TCC2 "teste" "4" "0.400000"
./TCC2 "teste" "4" "0.500000"
./TCC2 "teste" "4" "0.600000"
./TCC2 "teste" "4" "0.700000"
./TCC2 "teste" "4" "0.800000"
./TCC2 "teste" "4" "0.900000"
./TCC2 "teste" "4" "1.000000"
./TCC2 "teste" "8" "0.000000"
./TCC2 "teste" "8" "0.100000"
./TCC2 "teste" "8" "0.200000"
./TCC2 "teste" "8" "0.300000"
./TCC2 "teste" "8" "0.400000"
./TCC2 "teste" "8" "0.500000"
./TCC2 "teste" "8" "0.600000"
./TCC2 "teste" "8" "0.700000"
./TCC2 "teste" "8" "0.800000"
./TCC2 "teste" "8" "0.900000"
./TCC2 "teste" "8" "1.000000"

