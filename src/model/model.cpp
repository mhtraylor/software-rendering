#include "model.hpp"

Model::Model(const char *filename) : vertices_(), faces_()
{
    std::cout << "...loading OBJ file " << filename;

    std::ifstream in;
    in.open(filename, std::ifstream::in);

    if (in.fail())
    {
        std::cout << ": FAILED" << std::endl;
        return;
    }

    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream stream(line.c_str());

        char tmp0;

        if (!line.compare(0, 2, "v "))
        {
            stream >> tmp0;

            sf::Vector3f v;
            stream >> v.x;
            stream >> v.y;
            stream >> v.z;

            vertices_.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector< int > f;
            int tmp1, i;

            stream >> tmp0;

            while (stream >> i >> tmp0 >> tmp1 >> tmp0 >> tmp1)
            {
                i--;
                f.push_back(i);
            }

            faces_.push_back(f);
        }
    }

    std::cout << ": DONE (" << vertices_.size() << " vertices, " << faces_.size() << " faces)" << std::endl;
}

Model::~Model()
{}

int Model::num_vertices()
{
    return (int)vertices_.size();
}

int Model::num_faces()
{
    return (int)faces_.size();
}

std::vector< int > Model::get_face(int i)
{
    return faces_[i];
}

sf::Vector3f Model::get_vertex(int i)
{
    return vertices_[i];
}