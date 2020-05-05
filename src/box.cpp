//
// box.cpp
// Nathanael Epps
//

#include "box.hpp"
using namespace sf;

#include <cstdlib>
#include <ctime>

static int randint(int lo, int hi)
{
    static bool init = false;
    if (!init) {
        srand(time(NULL));
        init = true;
    }
    
    if (lo > hi)
        return 0;
    else
        return rand() % (hi - lo + 1) + lo;
}

static Color randColor()
{
    /*int r, g, b;
    r = randint(50, 255);
    if (r > 150) {
	g = 50;
	b = 50;
    }
    else {
	g = randint(50, 255);
	if (g > 150) {
	    r = 50;
	    b = 50;
	}
	else {
	    b = randint(150, 255);
	}
    }*/

    int r = randint(0, 255);
    int g = randint(5, 200);
    int b = randint(45, 234);

    return Color(r, g, b);
}

static float randSize()
{
    int dec = randint(0, 9);
    int whole = 1;
    
    while (true)
    {
        if (dec != 0 || whole != 0)
            break;
        
        dec = randint(0, 9);
        whole = randint(0, 3);
    }
    
    return (dec / 10.f) + whole;
}

Box::Box(b2World& world, b2Vec2 b2Bounds, Vector2f sfBounds)
{
    b2BodyDef def;
    b2PolygonShape shape;
    b2FixtureDef fixDef;
    
    scale = sfBounds.x / b2Bounds.x;
    
    (void) randSize(); // silence warning
    
    // const float size = randSize();
    const float size = 2.f;

    def.position = b2Vec2(b2Bounds.x / 2, b2Bounds.y / 5); // position
    def.type = b2_dynamicBody;
    body = world.CreateBody(&def);
    shape.SetAsBox(size / 2.f, size / 2.f); // size
    fixDef.shape = &shape;
    fixDef.density = 0.7f;
    fixDef.friction = 0.5f;
    fixture = body->CreateFixture(&fixDef);
    
    rect.setSize(Vector2f(size * scale, size * scale)); // size
    rect.setOrigin(rect.getSize().x / 2, rect.getSize().y / 2);
    rect.setPosition((b2Bounds.x / 2) * scale, (b2Bounds.y / 5) * scale); // position
    
    rect.setFillColor(randColor());
    
    body->SetAngularVelocity(randint(0, 15));
}

void Box::update()
{
    rect.setPosition(body->GetPosition().x * scale,
                     body->GetPosition().y * scale);
    
    rect.setRotation(body->GetAngle() * (180.f / b2_pi));
}

void Box::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(rect, states);
}
