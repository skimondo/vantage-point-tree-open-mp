# Rapport

Auteur:
Dominique Elias (ELID14019800)

## Parallelisation

### Étape 1 - concurrent_vector

D'abord il faut rendre le vecteur `nodes_` `tbb::concurrent_vector` pour permettre l'ajout de noeuds en parallèle.

```cpp
// vpparallel.h
tbb::concurrent_vector<VPNode> nodes_;
```

### Étape 2 - makeNode atomique

Ensuite, il faut rendre la fonction `makeNode(int item)` atomique, car elle retourne l'index essentiel pour l'arbre, pour faire cela on n'utilise plus `.size()` qui peut ne pas donner le resultat attendu en parallèle, on utilise donc l'itérateur retourner par `push_back` pour obtenir l'index du noeud ajouté, ce qui rend cette fonction atomique.

```cpp
int VPTreeParallel::makeNode(int item) {
  auto it = nodes_.push_back(VPNode(item));
  return static_cast<int>(std::distance(nodes_.begin(), it));
}
```

### Étape 3 - tbb::task_group

On peut maintenant paralléliser la fonction `build` en utilisant `tbb::task_group` pour créer les noeuds en parallèle. Plus précisement une tâche pour la partie gauche et une tâche pour la partie droite de chaque noeud.

```cpp
tbb::task_group g;
g.run([&] {
    n.left = makeTree(lower + 1, median);
});
g.run([&] {
    n.right = makeTree(median, upper);
});
g.wait();
```

### Étape 4 - granularité

Pour finir, on doit controler la granularité de la parallélisation, pour cela on utilise une variable `max_depth` qui limite la profondeur de l'arbre pour la parallélisation.

```cpp
// vpparallel.h
void setMaxDepth(int max_depth);
int m_max_depth = std::numeric_limits<int>::max(); // default
```

et on modifie la fonction `makeTree` pour prendre en compte cette limite.

```cpp
// vpparallel.cpp
if (depth < max_depth) {
    tbb::task_group g;
    g.run([&] {
       n.left = makeTree(lower + 1, median, depth + 1, max_depth);
    });
    g.run([&] {
        n.right = makeTree(median, upper, depth + 1, max_depth);
    });
    g.wait();
} else {
    n.left = makeTree(lower + 1, median, depth + 1, max_depth);
    n.right = makeTree(median, upper, depth + 1, max_depth);
}
```

## Vérification

Avec les tests fournis, on peut vérifier que la parallélisation fonctionne correctement, il suffit d'augmenter le nombre de noeuds pour vérifier aucun problème de concurrence. l'outil `helgrind` nous a permis aussi de voir qu'il n'y a pas de problème de concurrence.

```sh

```cpp

```sh

```cpp

```sh
./build/Desktop-Release/bin/bench_vptree 
cmake -G Ninja -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake -B build-release -DENABLE_THREAD_SANITIZER=ON 
cmake --build build-release
srun -c 8 ./build-release/bin/bench_vptree 
scp skimondo@inf5171.calculquebec.cloud:execution_time_vs_granularity.png ~/Documents

```

```sh
module load python
pip install pandas
pip install matplotlib
```

```sh
awk '!seen[$1]++' granularite.dat > granularite.dat
```