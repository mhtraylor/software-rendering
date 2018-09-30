#include "main.hpp"
#include "model/model.hpp"

const int FRAME_WIDTH   = 800;
const int FRAME_HEIGHT  = 800;

Model * model = NULL;

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
    int y  = y0;

    float de = std::abs(dy / float(dx));
    float e  = 0.f;

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

    for (int i = 0; i < model->num_faces(); i++)
    {
        std::vector< int > face = model->get_face(i);

        for (int j = 0; j < 3; j++)
        {
            sf::Vector3f v0 = model->get_vertex(face[j]);
            sf::Vector3f v1 = model->get_vertex(face[(j + 1) % 3]);

            int x0 = (v0.x + 1.f) * FRAME_WIDTH / 2.f;
            int y0 = (v0.y + 1.f) * FRAME_HEIGHT / 2.f;
            int x1 = (v1.x + 1.f) * FRAME_WIDTH / 2.f;
            int y1 = (v1.y + 1.f) * FRAME_HEIGHT / 2.f;

            plot_line(x0, y0, x1, y1, sf::Color::White, image);
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