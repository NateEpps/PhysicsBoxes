//
// box.hpp
// Nathanael Epps
//

#ifndef BOX_HPP
#define BOX_HPP

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

class Box : public sf::Drawable
{
public:
    Box(b2World&, b2Vec2, sf::Vector2f);
    
    void update();
    
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const override;
    
    inline b2Body* getBody() const
    { return body; }
    
private:
    mutable b2Body* body;
    b2Fixture* fixture;
    sf::RectangleShape rect;
    float scale;
};

#endif
