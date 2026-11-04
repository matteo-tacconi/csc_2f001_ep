#include "location.h"
#include <cmath>
#include <numbers>

// Field initializers following a colon[cite: 6]
location_t::location_t(std::string name, double latitude, double longitude)
    : name{name}, latitude{latitude}, longitude{longitude}, neighbors{nullptr}, nb_neighbors{0} {
}

location_t::~location_t() {
    // Free the dynamically allocated array of pointers[cite: 35]
    delete[] neighbors;
}

void location_t::set_neighbors(location_t* list[]) {
    neighbors = list;
    nb_neighbors = 0;
    
    // Implicit parameter 'this' used to access fields[cite: 26]
    if (neighbors != nullptr) {
        while (neighbors[nb_neighbors] != nullptr) {
            nb_neighbors++;
        }
    }
}

double location_t::distance_to(location_t* to) {
    double R = 6378.0;
    double lat1 = latitude;
    double long1 = longitude;
    double lat2 = to->latitude;
    double long2 = to->longitude;

    return R * (std::numbers::pi / 2.0 - std::asin(std::sin(lat2) * std::sin(lat1) + std::cos(long2 - long1) * std::cos(lat2) * std::cos(lat1)));
}