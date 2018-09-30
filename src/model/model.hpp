#ifndef MODEL_HPP
#define MODEL_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <SFML/Graphics.hpp>

class Model
{
    private:
        std::vector< sf::Vector3f > vertices_;
        std::vector< std::vector< int > > faces_;

    public:
        Model(const char *filename);
        ~Model();

        int num_vertices();
        int num_faces();

        sf::Vector3f get_vertex(int i);
        std::vector< int > get_face(int i);
};

#endif // MODEL_HPP