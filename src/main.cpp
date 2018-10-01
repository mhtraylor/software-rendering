#include "main.hpp"
#include "model/model.hpp"

const int FRAME_WIDTH = 800;
const int FRAME_HEIGHT = 800;

Model *model = NULL;

void plot_line(int x0, int y0, int x1, int y1, sf::Color color, sf::Image &image)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int y = y0;

    float de = std::abs(dy / float(dx));
    float e = 0.f;

    for (int x = x0; x <= x1; x++)
    {
        if (steep)
        {
            image.setPixel(x, y, color); /* TODO: why is this flipped? Should be (y,x) here... */
        }
        else
        {
            image.setPixel(y, x, color); /* TODO: why is this flipped? Should be (x,y) here... */
        }

        e += de;
        if (e > 0.5f)
        {
            y += (y1 > y0 ? 1 : -1);
            e -= 1.f;
        }
    }
}

sf::IntRect get_triangle_bounds(sf::Vector2i v0, sf::Vector2i v1, sf::Vector2i v2)
{
    int l = std::min(v0.x, std::min(v1.x, v2.x));
    int t = std::min(v0.y, std::min(v1.y, v2.y));
    int r = std::max(v0.x, std::max(v1.x, v2.x));
    int b = std::max(v0.y, std::max(v1.y, v2.y));

    return sf::IntRect(l, t, r + l, b + t);
}

// vector library
sf::Vector3f cross(sf::Vector3f a, sf::Vector3f b)
{
    float x = (a.y * b.z) - (a.z * b.y);
    float y = (a.z * b.x) - (a.x * b.z);
    float z = (a.x * b.y) - (a.y * b.x);

    return sf::Vector3f(x, y, z);
}

float dot(sf::Vector3f a, sf::Vector3f b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float norm(sf::Vector3f &v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

sf::Vector3f normalize(sf::Vector3f v)
{
    return v * (1.f / norm(v));
}

void normalized(sf::Vector3f &v)
{
    v *= (1.f / norm(v));
}

sf::Vector3f bary(sf::Vector2i v0, sf::Vector2i v1, sf::Vector2i v2, sf::Vector2i b)
{
    sf::Vector3f u = cross(
        sf::Vector3f(v2.x - v0.x, v1.x - v0.x, v0.x - b.x),
        sf::Vector3f(v2.y - v0.y, v1.y - v0.y, v0.y - b.y));

    if (std::abs(u.z) < 1)
        return sf::Vector3f(-1.f, 1.f, 1.f);
    return sf::Vector3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void plot_triangle(sf::Vector2i v0, sf::Vector2i v1, sf::Vector2i v2, sf::Color color, sf::Image &image)
{
    // barycentric method
    sf::IntRect b = get_triangle_bounds(v0, v1, v2);

    sf::Vector2i p;

    for (p.x = b.left; p.x <= b.width; p.x++)
    {
        for (p.y = b.top; p.y <= b.height; p.y++)
        {
            sf::Vector3f s = bary(v0, v1, v2, p);

            if (s.x < 0 || s.y < 0 || s.z < 0)
                continue;
            image.setPixel(p.y, p.x, color); // TODO: again, flipped :(
        }
    }
}

int main()
{
    std::cout << "Software Rendering Demos" << std::endl;

    sf::RenderWindow window(sf::VideoMode(FRAME_WIDTH, FRAME_HEIGHT), "SFML");

    std::cout << "Loading data..." << std::endl;

    model = new Model("content/obj/african_head.obj");

    std::cout << "Loading data: DONE" << std::endl;

    std::cout << "Rendering..." << std::endl;

    sf::Image image;
    image.create(FRAME_WIDTH, FRAME_HEIGHT, sf::Color::Black);

    // Wireframe
    // for (int i = 0; i < model->num_faces(); i++)
    // {
    //     std::vector< int > face = model->get_face(i);

    //     for (int j = 0; j < 3; j++)
    //     {
    //         sf::Vector3f v0 = model->get_vertex(face[j]);
    //         sf::Vector3f v1 = model->get_vertex(face[(j + 1) % 3]);

    //         int x0 = (v0.x + 1.f) * FRAME_WIDTH / 2.f;
    //         int y0 = (v0.y + 1.f) * FRAME_HEIGHT / 2.f;
    //         int x1 = (v1.x + 1.f) * FRAME_WIDTH / 2.f;
    //         int y1 = (v1.y + 1.f) * FRAME_HEIGHT / 2.f;

    //         plot_line(x0, y0, x1, y1, sf::Color::White, image);
    //     }
    // }

    // Flat-shading
    sf::Vector3f light(0.f, 0.f, -1.f);

    for (int i = 0; i < model->num_faces(); i++)
    {
        std::vector< int > face = model->get_face(i);
        sf::Vector2i screen[3];
        sf::Vector3f world[3];
        // for each vertex of the face, get world & screen coords
        for (int j = 0; j < 3; j++)
        {
            sf::Vector3f v = model->get_vertex(face[j]);
            screen[j] = sf::Vector2i((v.x + 1.f) * FRAME_WIDTH / 2.f, (v.y + 1.f) * FRAME_HEIGHT / 2.f);
            world[j] = v;
        }

        // simple lighting
        sf::Vector3f n = cross(world[2] - world[0], world[1] - world[0]);
        normalized(n);

        float intensity = dot(n, light);

        if (intensity > 0)
        {
            sf::Color color(intensity * 255, intensity * 255, intensity * 255, 255);
            plot_triangle(screen[0], screen[1], screen[2], color, image);
        }
    }

    delete model;

    sf::Texture texture;
    texture.create(FRAME_WIDTH, FRAME_HEIGHT);
    texture.update(image);

    sf::Sprite sprite;
    // sprite.setPosition(sf::Vector2f(0, 0));
    sprite.setTexture(texture);

    std::cout << "Rendering: DONE" << std::endl;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}