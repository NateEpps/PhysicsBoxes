//
// main.cpp
// Nathanael Epps
//

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <Config.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "box.hpp"

using namespace sf;
using std::cout;
using std::string;

template <class T>
std::ostream& operator<<(std::ostream& stream, Vector2<T> v)
{
    stream << "(" << v.x << ", " << v.y << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, b2Vec2 v)
{
    stream << "(" << v.x << ", " << v.y << ")";
    return stream;
}

struct Wall : public Drawable
{
    b2BodyDef def;
    b2Body* body;
    b2PolygonShape shape;
    b2FixtureDef fixDef;
    b2Fixture* fixture;
    RectangleShape sfShape;
    
    virtual void draw(RenderTarget& target, RenderStates states) const override
    { target.draw(sfShape, states); }
};

#ifdef __APPLE__
inline
#endif
string numToString(size_t st)
{
#ifdef __APPLE__
    return (std::stringstream() << st).str();
#else
    std::stringstream ss;
    ss << st;
    return ss.str();
#endif
}

int main()
{
    cout << "Hello World!\n";

    // Set up SFML
    RenderWindow window;
    VideoMode vm = VideoMode::getFullscreenModes().at(0);
    Vector2f sfBounds = Vector2f(vm.width, vm.height);
    
    cout << "VideoMode: " << sfBounds << "\n";
    
    window.create(vm, "(null)", Style::Fullscreen);
    // window.setKeyRepeatEnabled(false);
    
    // Set up Box2D
    b2Vec2 gravity = b2Vec2(0, 9.8);
    
    b2World world(gravity);
    
    b2Vec2 b2Bounds;
    b2Bounds.x = 50;
    b2Bounds.y = sfBounds.y * b2Bounds.x / sfBounds.x; // proportional
#ifdef __linux__
    // with weird scales try getting rid of fractional part
    b2Bounds.y = (float) ((int) b2Bounds.y);
#endif
    
    // Scale!
    const float scale = sfBounds.x / b2Bounds.x;
    
    cout << "Dimensions:\n\tSFML: " << sfBounds << "\tBox2D: " << b2Bounds << "\n";
    cout << "Scaling factor: " << scale << "\n";
    
    // create the ground and walls
    constexpr float WallFriction = 0.5f;
    
    Wall ground;
    ground.def.position = b2Vec2(b2Bounds.x / 2.f, b2Bounds.y - 0.5f); // position
    ground.def.type = b2_staticBody;
    ground.body = world.CreateBody(&ground.def);
    ground.shape.SetAsBox(b2Bounds.x/2.f, 1.f/2.f); // size
    ground.fixDef.shape = &ground.shape;
    ground.fixDef.friction = WallFriction;
    ground.fixDef.density = 0.0f;
    ground.fixture = ground.body->CreateFixture(&ground.fixDef);
    ground.sfShape.setSize(Vector2f(b2Bounds.x * scale, 1 * scale)); // size
    ground.sfShape.setOrigin(ground.sfShape.getSize().x / 2, ground.sfShape.getSize().y / 2);
    ground.sfShape.setPosition(b2Bounds.x / 2.f * scale, (b2Bounds.y - 0.5f) * scale);
    ground.sfShape.setFillColor(Color(0, 150, 70));
    
    Wall left;
    left.def.position = b2Vec2(0.5, b2Bounds.y / 2.f); // position
    left.def.type = b2_staticBody;
    left.body = world.CreateBody(&left.def);
    left.shape.SetAsBox(1.f / 2.f, b2Bounds.y / 2.f); // size
    left.fixDef.shape = &left.shape;
    left.fixDef.density = 0.0f;
    left.fixDef.friction = WallFriction;
    left.fixture = left.body->CreateFixture(&left.fixDef);
    left.sfShape.setSize(Vector2f(1 * scale, b2Bounds.y * scale)); // size
    left.sfShape.setOrigin(left.sfShape.getSize().x / 2, left.sfShape.getSize().y / 2);
    left.sfShape.setPosition(0.5 * scale, (b2Bounds.y / 2.f) * scale); // position
    left.sfShape.setFillColor(ground.sfShape.getFillColor());
    
    Wall right;
    right.def.position = b2Vec2(b2Bounds.x - 0.5, b2Bounds.y / 2.f); // position
    right.def.type = b2_staticBody;
    right.body = world.CreateBody(&right.def);
    right.shape.SetAsBox(1.f / 2.f, b2Bounds.y / 2.f); // size
    right.fixDef.shape = &right.shape;
    right.fixDef.density = 0.0f;
    right.fixDef.friction = WallFriction;
    right.fixture = right.body->CreateFixture(&right.fixDef);
    right.sfShape.setSize(Vector2f(1 * scale, b2Bounds.y * scale)); // size
    right.sfShape.setOrigin(right.sfShape.getSize().x / 2, right.sfShape.getSize().y / 2);
    right.sfShape.setPosition((b2Bounds.x - 0.5) * scale, (b2Bounds.y / 2.f) * scale);
    right.sfShape.setFillColor(left.sfShape.getFillColor());
    
    // create container for boxes
    std::vector<Box> boxes;
    constexpr size_t limit = 10000;
    Clock timer;
    
    // load the font
    Font sansation;
    if (!sansation.loadFromFile(PREFIX + "sansation.ttf")) {
        std::cerr << "Unable to load file!\n";
        return -1;
    }
    
    // create text obj
    Text drawCounter;
    drawCounter.setFont(sansation);
    drawCounter.setString("Boxes: " + numToString(boxes.size()));
    drawCounter.setCharacterSize(45);
    drawCounter.setPosition(50, 50);
    
    // running loop
    while (window.isOpen())
    {
        // poll events
        Event e;
        while (window.pollEvent(e)) {
            // handle close
            if (e.type == Event::Closed) {
                window.close();
            }
            else if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    window.close();
                }
                else if (e.key.code == Keyboard::Return) {
                    for (Box& b : boxes)
                        world.DestroyBody(b.getBody());
                    boxes.clear();
                    drawCounter.setString("Boxes: " + numToString(boxes.size()));
                }
                else if (e.key.code == Keyboard::Space && boxes.size() < limit) {
                    boxes.push_back(Box(world, b2Bounds, sfBounds));
                    drawCounter.setString("Boxes: " + numToString(boxes.size()));
                }
                else if (e.key.code == Keyboard::Down) {
                    View v = window.getView();
                    View nv (v.getCenter(), Vector2f(v.getSize().x * 2, v.getSize().y * 2));
                    
                    window.setView(nv);
                }
                else if (e.key.code == Keyboard::Up) {
                    View v = window.getView();
                    View nv (v.getCenter(), Vector2f(v.getSize().x / 2, v.getSize().y / 2));
                    
                    window.setView(nv);

                }
                else if (e.key.code == Keyboard::Left || e.key.code == Keyboard::Right) {
                    window.setView(window.getDefaultView());
                }
            }
        }
        
        // update
        world.Step(timer.restart().asSeconds(), 8, 3);
        
        // draw stuff
        window.clear(Color(50, 50, 50));
        window.draw(ground);
        window.draw(left);
        window.draw(right);
        for (Box& b : boxes) {
            b.update();
            window.draw(b);
        }
        window.draw(drawCounter);
        window.display();
    }
}
