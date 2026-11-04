# My vacation in Bilbao

The objective of this exercise is to familiarize you with your first objects and constructors. The exercise also introduces you graphs by implementing a simple version of the A* algorithm, which efficiently finds a path between two cities (technically, a path between two nodes in a weighted graph).

In this exercise, we implement a small navigation application that allows you to travel by car. This application enables you to calculate an efficient route from one city to another using the A* (pronounce A star) algorithm. For this, you will need to work with weighted graphs. A graph is a mathematical structure where nodes are connected by edges that carry a weight. In our case, the nodes represent cities, the edges symbolize roads, and the weight of an edge represents the distance to travel on the road.

> **Note:**  
> This lab is the most complex lab of the course. The complexity arises from the complexity of the A* algorithm and from the complexity of the data structures used to implement the algorithm. Since this lab is more complex than the others, you have two weeks to finish it instead of one. Do you best to finish the lab: you will learn a lot and start to really master the C and C++ programming languages.

## Preparation

### Question 1

In your `csc_2f001_ep` repository, create a new directory called `lab4`. In this directory, creates a `main.cpp` file, which uses `std::cout` to print `Hello world!!!` in the terminal. Create a `Makefile` that compiles `main.cpp` into the `bilbao` binary. Your `Makefile` has to generate first the intermediate object files, and then to generate the binary. For that, you can start from the `Makefile` that you wrote in lab 3.

> **Warning:**  
> Add the option `-std=c++20` when you compile a file with `g++`. This option instructs `g++` to use the `c++20` standard and not an older one because in this course, we rely on this standard. In the next labs, don't forget to add this option when you compile a C++ source file.

### Question 2

Add the files `main.cpp` and `Makefile` to your repository. Commit the changes and push them on `gitlab`. In the remainder of the lab, when you create a new source file, don't forget to add it to the repository.

## The cities and the roads

In this exercise, you will design a structure named `location_t`, which represents a city. A city consists of:

* a name (an `std::string`),
* a latitude and a longitude (two `doubles`),
* an array of neighbors (a pointer to an array that contains `location_t*` elements),
* the number of neighbors (a `size_t`).

### Question 1

Write a structure `location_t` in a file `location.h`. The structure has a constructor that takes as argument the name, the latitude and the longitude of the city. Write also the implementation of the constructor in a file `location.cpp`. In the main function, copy the following code that creates some cities:

```cpp
  location_t palaiseau { "Palaiseau", 0.850219, 0.0386527 };
  location_t paris { "Paris", 0.852709, 0.041054 };
  location_t strasbourg { "Strasbourg", 0.837758, 0.122173 };
  location_t reims { "Reims", 0.855211, 0.069813 };
  location_t dijon { "Dijon", 0.820304, 0.087266 };
  location_t lemans { "Le Mans", 0.837865, 0.00348291 };
  location_t orleans { "Orléans", 0.836064, 0.0333227 };
  location_t angers { "Angers", 0.828655, -0.0098291 };
  location_t tours { "Tours", 0.827184, 0.0119527 };
  location_t bourges { "Bourges", 0.821719, 0.0418666 };
  location_t poitiers { "Poitiers", 0.812978, 0.00594066 };
  location_t limoges { "Limoges", 0.799948, 0.0220104 };
  location_t angouleme { "Angouleme", 0.796714, 0.00272685 };
  location_t bordeaux { "Bordeaux", 0.782567, -0.0101086 };
  location_t agen { "Agen", 0.77149, 0.0107576 };
  location_t toulouse { "Toulouse", 0.761045, 0.0252062 };
  location_t bayonne { "Bayonne", 0.759095, -0.0257408 };
  location_t pau { "Pau", 0.755642, -0.00647163 };
  location_t sansebastian { "San Sebastian", 0.756048, -0.034579 };
  location_t pampelune { "Pampelune", 0.74722, -0.0287242 };
  location_t bilbao { "Bilbao", 0.755082, -0.0512252 };
```

> **Note:**  
> The latitude and longitude are given in radians and not in degrees. This will simplify the code that computes the distance between two cities.

### Question 2

Add a method `void set_neighbors(location_t* list[])` to the structure `location_t`. This function takes as argument an array of cities that ends with a null pointer (`nullptr` in C++). Write the implementation of `set_neighbors` in `location.cpp`. This function has to:

* set the value of the field neighbors to list,
* and compute the `nb_neighbors` field of `location_t` by finding the `nullptr` in `list`.

In the main function, create the neighbors of our cities by copying the following code:

```cpp
  palaiseau.set_neighbors(new location_t*[] { &paris, nullptr });
  paris.set_neighbors(new location_t*[] { &palaiseau, &lemans, &orleans, &reims, &dijon, nullptr });
  reims.set_neighbors(new location_t*[] { &strasbourg, &dijon, nullptr });
  strasbourg.set_neighbors(new location_t*[] { &reims, &dijon, nullptr });
  dijon.set_neighbors(new location_t*[] { &reims, &paris, &bourges, nullptr });
  lemans.set_neighbors(new location_t*[] { &orleans, &tours, &angers, nullptr });
  orleans.set_neighbors(new location_t*[] { &lemans, &paris, &bourges, &tours, nullptr });
  angers.set_neighbors(new location_t*[] { &lemans, &tours, &poitiers, nullptr });
  tours.set_neighbors(new location_t*[] { &angers, &lemans, &orleans, &bourges, &poitiers, nullptr });
  bourges.set_neighbors(new location_t*[] { &limoges, &tours, &orleans, nullptr });
  poitiers.set_neighbors(new location_t*[] { &limoges, &angouleme, nullptr });
  limoges.set_neighbors(new location_t*[] { &agen, &angouleme, &poitiers, nullptr });
  angouleme.set_neighbors(new location_t*[] { &poitiers, &limoges, &agen, &bordeaux, nullptr });
  bordeaux.set_neighbors(new location_t*[] { &angouleme, &agen, &bayonne, nullptr });
  agen.set_neighbors(new location_t*[] { &toulouse, &pau, &bordeaux, &angouleme, &limoges, nullptr });
  toulouse.set_neighbors(new location_t*[] { &agen, &pau, nullptr });
  bayonne.set_neighbors(new location_t*[] { &bordeaux, &pau, &sansebastian, nullptr });
  pau.set_neighbors(new location_t*[] { &pampelune, &bayonne, &agen, &toulouse, nullptr });
  sansebastian.set_neighbors(new location_t*[] { &bayonne, &pampelune, &bilbao, nullptr });
  pampelune.set_neighbors(new location_t*[] { &bilbao, &sansebastian, &pau, nullptr });
  bilbao.set_neighbors(new location_t*[] { &sansebastian, &pampelune, nullptr });
```

### Question 3

As you can see in the code that we provide, the array of neighbors is allocated with `new`, but is never freed after. In order to avoid this memory leak, add a destructor to the structure `location_t`. The destructor is in charge of deleting the `neighbors` array (i.e., the array that is allocated with `new` in `main`). Since `neighbors` is an array, you have to delete it with `delete[] neighbors`.

### Question 4

Add a function `double distance_to(location_t* to)` to the structure `location_t`. This function returns the distance in kilometers between the city `this` (i.e., the receiver of the call) and the city `to`. The formula that computes this distance is:

```cpp
R * (std::numbers::pi/2 - std::asin(std::sin(lat2) * std::sin(lat1) + std::cos(long2 - long1) * std::cos(lat2) * std::cos(lat1)));
```

In this formula, R is the radius of the Earth (6378 km). To use `std::numbers::pi`, you have to include `numbers`, and to use the trigonometric functions, you have to include `cmath`.

Verify that your function is correct by computing the distance between Palaiseau and Paris. If this distance is equal to 18.816 km, it means that your code is correct, congratulations!

## A* algorithm

You will now implement the A* algorithm. The A* algorithm computes a path from a location `from` to a location `to`, while trying to minimize the distance.

Our implementation of the A* algorithm is not optimized. It considers an unordered set of nodes, in which each node represents a city. A node gives the minimal distance to the origin (`from`) and the total distance to reach the destination if a virtual direct road from the city to the destination (`to`) would have existed (distance as the crow flies). A node also memorizes the previous city that we used to reach it, which will be used at the end of the algorithm to display the path. In the exercise, the set of nodes is represented as a linked list, which means that each node has also a pointer the a next node. As a result, a node has the following fields:

* `node_t* next`: the next node in the unordered set.
* `location_t* location`: the city represented by the node.
* `node_t* from`: the node that represents the city that was used to find the shortest path to reach `location`.
* `double from_origin`: the distance from the origin when we arrive to `location` from `from`.
* `double total`: the total distance to reach the destination if a direct path from `location` to the destination would have existed.
* `bool visited`: a boolean that indicates if we have already visited the city (see below).

> **Tips:**  
> In detail, the A* algorithm works as follow:
> * Initially, the set only contains the origin.
> * At each step, the algorithm finds the node `base`, which is the non visited node that minimizes the distance to reach the destination. In other words, `base` is the non visited node with the smallest `total` field.
> * If `base` represents the destination, the algorithm found a path and thus stops.
> * If such a `base` does not exist (i.e., all the nodes are visited), it means that we visited all the cities reachable from the origin without having found a path that reaches the destination. The algorithm thus also stops since we cannot reach the destination from the origin.
> * If `base` exists and does not represent the destination, the algorithm marks `base` has visited, which avoids restarting from `base` later.
> * The algorithm then explores the possible paths that start from `base`. For each neighbor `cur` of `base`:
>   * The algorithm computes `from_origin`, which is the distance to reach `cur` when we arrive from `base`. `from_origin` is thus equal to `base->from_origin` plus the distance between `base` and `cur`.
>   * The algorithm computes `total`, which is the distance to reach the destination when we arrive from `base` if a direct road from `cur` to the destination would have existed. `total` is thus equal to `from_origin` plus the distance between `cur` and the destination.
>   * The algorithm then inspects the set of nodes to find if `cur` was already reached through another path. Here, we have two possibilities:
>     * `cur` is not in the set. In this case, you have to create a new `node` `n` to represent `cur`. The `from` field of `n` is `base` since we arrive to `cur` from `base`. The `from_origin` and `total` fields of `n` are the ones we just computed. `n` is not yet visited.
>     * `cur` is already in the set. We have again two possibilities.
>       * The distance between the origin and `cur` that was computed previously is lower than the `from_origin` distance computed when we reach `cur` from `base`. In this case, we don't have anything to do: we already found a better path to reach `cur`.
>       * If this is not the case, we have to update the node: its `from`, `from_origin` and `total` fields. The node was also maybe already previously visited. We have thus to mark the node as non visited again in order to recompute the distance to reach its neighbors.
> * The algorithm has finally to continue with the next `base`.

### Question 1

In a file `shortest_path.h`, give the definition of the structure node. Give also the definition of a structure `shortest_path_t`, which will be used to compute the shortest path from an origin to a destination. This structure has the following fields:

* `node_t* head`: the set of nodes (a pointer to the first node of the set),
* `node_t* base`: the current base,
* `location_t* from`: the origin,
* `location_t* to`: the destination.

The `shortest_path_t` structure also has the following methods:

* A constructor that takes `from` and `to` as arguments and initializes the fields,
* `void compute()`: computes the shortest path from `from` to `to`,
* `void display()`: displays this path if it was found.

### Question 2

Give the implementation of `node_t` and `shortest_path_t` in a file `shortest_path.cpp`. If you try to find a path from Palaiseau to Bilbao, your program should outputs this path:

```text
path from Palaiseau to Bilbao
  * Palaiseau at 0 km from Palaiseau
  * Paris at 18.816 km from Palaiseau
  * Orléans at 129.914 km from Palaiseau
  * Tours at 237.795 km from Palaiseau
  * Poitiers at 332.101 km from Palaiseau
  * Angouleme at 436.802 km from Palaiseau
  * Bordeaux at 543.87 km from Palaiseau
  * Bayonne at 709.778 km from Palaiseau
  * San Sebastian at 755.108 km from Palaiseau
  * Bilbao at 832.632 km from Palaiseau
```

If you try to come back from Bilbao, there is a missing road on the way back, and you are thus definitely stuck at Bilbao. For this reason, if you try to find a path from Bilbao to Palaiseau, you program should output:

```text
unable to find a path from Bilbao to Palaiseau
```

### Question 3

In `shortest_path_t`, add a destructor in charge of deleting the nodes that were allocated in the `compute` method.

### Question 4

Submit your work. For that, start by running `git status` to identify the untracked files and to add them to the repository with `git add`. Then, commit your code (`git commit`) and push the commit to the gitlab repository (`git push`).

🎉 **You are now able to develop complex applications in C++, congratulations!**

If you want to test more paths, here are some outputs that you can generate. Note that many roads are missing when you go from the south to the north.

```text
path from Strasbourg to Bordeaux
  * Strasbourg at 0 km from Strasbourg
  * Dijon at 187.115 km from Strasbourg
  * Bourges at 384.641 km from Strasbourg
  * Limoges at 548.625 km from Strasbourg
  * Angouleme at 636.903 km from Strasbourg
  * Bordeaux at 743.971 km from Strasbourg
path from Tours to Reims
  * Tours at 0 km from Tours
  * Orléans at 107.881 km from Tours
  * Paris at 218.979 km from Tours
  * Reims at 340.54 km from Tours
unable to find a path from Bilbao to Palaiseau
```
