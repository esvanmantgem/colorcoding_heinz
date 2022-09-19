//
// Created by ishuena on 09.10.17.
//

#ifndef HEINZ_VERTICES_H
#define HEINZ_VERTICES_H


#include <vector>
#include <unordered_map>
#include <cfloat>
#include "Vertex.h"
#include <algorithm>


namespace colcod {
    class Vertices {
    public:
        std::vector<Vertex> vertices;
        //Vertices();
        //void add_vertex(Vertex v);

        //const unsigned long size() const;
        //const int get_weight() const;
        //const std::string string() const;
        //const bool has_vertex(Vertex v) const;

        //const Vertex get_vertex(int i) const;

    private:
        //std::vector<Vertex> vertices;
        double weight;


    public:
        Vertices() {
            weight = -DBL_MAX;
        }

        virtual ~Vertices() {}

        void add_vertex(Vertex v) {
            // Hack : oplossen in programma!
            if (std::find(vertices.begin(), vertices.end(), v) == vertices.end()) {
            //if (!has_vertex(v)) {
                vertices.push_back(v);
                if (weight == -DBL_MAX) weight = 0;
                weight += v.get_weight();
            }
        }

        const unsigned long size() const {
            return vertices.size();
        }

        const Vertex get_vertex(int i) const {
            return vertices.at(i);
        }

        const Vertex get_vertex(std::string name) {
            for (auto it : vertices) {
                if (name.compare(it.get_name()) == 0) {
                    return it;
                }
            }
        }

        const bool has_vertex(Vertex v) const {
            //for (auto it : vertices) {
            //    if (v.get_name().compare(it.get_name()) == 0) return true;
            //}
            //return false;
            return std::find(vertices.begin(), vertices.end(), v) == vertices.end();
        }

        const double get_weight() const {
            return weight;
        }

        const std::string string() const {
            std::string result = "";
            for (auto it : vertices) {
                result += it.get_name() + " ";
            }
            return result;
        }

    };
}



#endif //HEINZ_VERTICES_H
