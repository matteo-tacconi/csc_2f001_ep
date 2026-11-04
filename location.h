#pragma once
#include <string>
#include <cstddef>

struct location_t {
    std::string name;
    double latitude;
    double longitude;
    location_t** neighbors;
    size_t nb_neighbors;

    // Constructor to initialize fields[cite: 5]
    location_t(std::string name, double latitude, double longitude);
    
    // Destructor to free dynamically allocated arrays[cite: 12]
    ~location_t();

    // Instance methods[cite: 28]
    void set_neighbors(location_t* list[]);
    double distance_to(location_t* to);
};