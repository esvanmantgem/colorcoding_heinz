//
// Created by ishuena on 09.10.17.
//

#ifndef HEINZ_VERTEX_H
#define HEINZ_VERTEX_H

#include <string>
#include <vector>
#include <boost/functional/hash.hpp>
#include "ColorSet.h"

namespace colcod {
    class Vertex {

    public:
/*
        Vertex(std::string n, double w, uint32_t c);
        Vertex(std::string n, double w);

        const std::string get_name() const;
        double get_weight();
        uint32_t get_color();
        unsigned long get_number_of_neighbors();
        Vertex get_neighbor(int i);
        void set_neighbor(Vertex v);
        bool operator==(Vertex const& other) const;
*/
    private:
        std::string name;
        double weight;
        uint32_t color;
        std::vector<Vertex> neighbors;

    public:
    Vertex(std::string n, double w) {
        name = n;
        weight = w;
    }

    Vertex(std::string n, double w, uint32_t c) {
        name = n;
        weight = w;
        color = c;
    }

    const std::string get_name() const {
        return name;
    }

    double get_weight() {
        return weight;
    }

    uint32_t get_color() {
        return color;
    }

    unsigned long get_number_of_neighbors() {
        return neighbors.size();
    }

    Vertex get_neighbor(int i) {
        return neighbors.at(i);
    }


    void set_neighbor(Vertex v) {
        neighbors.push_back(v);
    }


    bool operator==(Vertex const &other) const {
        return name.compare(other.name) == 0;
        //return id == other.id;
    }

    };
}


#endif //HEINZ_VERTEX_H
