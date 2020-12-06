# Genetic Algorithm for Traveling Salesman Problem
Using three different crossover methods of genetic algorithm to solve TSP problem.
Testing data is from : http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/
eil51.tsp

## Partially Mapped Crossover(PMX)
1. Select two random number c1 and c2 (c1 <= c2).
2. Exchange the genes from c1 to c2.
3. Fill the original genes to the chromosome which is no conflict with the exchange part.
   if conflict remain the position then fill the gene from opposite chromosome and
   still check the conflict with the exchange part.

## Order Crossover(OX)
1. Select two random number c1 and c2 (c1 <= c2).
2. Fill the genes by opposite chromosome from c2+1 to c2 and check 
   conflict with the original genes from c1 to c2. If conflict happened
   just skip this gene from opposite  and choose next candidate genes.
   
## Cycle Crossover(CX)
1. Select one random number c1 be the start point.
2. From c1 remain self gene, and set c1 be the opposite chromosome's gene number.
3. Repeat 2. until the remain genes is a cycle.
4. Fill the other genes from opposite chromosome which has not been filled.

## Experimental result
... 