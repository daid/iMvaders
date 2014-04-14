#include "Collisionable.h"
#include "Renderable.h"
#include "vectorUtils.h"

class CollisionDebugDraw : public Renderable, public b2Draw
{
    sf::RenderTarget* renderTarget;
public:
    CollisionDebugDraw()
    {
        SetFlags(e_shapeBit | e_jointBit | e_centerOfMassBit);
        CollisionManager::world->SetDebugDraw(this);
    }
    
    virtual void preRender(sf::RenderTarget& window) {}
    virtual void render(sf::RenderTarget& window) {}
    virtual void postRender(sf::RenderTarget& window)
    {
        renderTarget = &window;
        CollisionManager::world->DrawDebugData();
    }

	/// Draw a closed polygon provided in CCW order.
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
        sf::VertexArray a(sf::LinesStrip, vertexCount+1);
        for(int32 n=0; n<vertexCount; n++)
            a[n].position = sf::Vector2f(vertices[n].x, vertices[n].y);
        a[vertexCount].position = sf::Vector2f(vertices[0].x, vertices[0].y);
        renderTarget->draw(a);
	}

	/// Draw a solid closed polygon provided in CCW order.
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
        sf::VertexArray a(sf::LinesStrip, vertexCount+1);
        for(int32 n=0; n<vertexCount; n++)
        {
            a[n].position = sf::Vector2f(vertices[n].x, vertices[n].y);
            a[n].color = sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
        }
        a[vertexCount].position = sf::Vector2f(vertices[0].x, vertices[0].y);
        renderTarget->draw(a);
	}

	/// Draw a circle.
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
	{
        sf::CircleShape shape(radius, 16);
        shape.setOrigin(radius, radius);
        shape.setPosition(sf::Vector2f(center.x, center.y));
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255));
        shape.setOutlineThickness(1.0);
        renderTarget->draw(shape);
	}
	
	/// Draw a solid circle.
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
	{
        sf::CircleShape shape(radius, 16);
        shape.setOrigin(radius, radius);
        shape.setPosition(sf::Vector2f(center.x, center.y));
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255));
        shape.setOutlineThickness(1.0);
        renderTarget->draw(shape);
	}
	
	/// Draw a line segment.
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
	{
        sf::VertexArray a(sf::Lines, 2);
        a[0].position = sf::Vector2f(p1.x, p1.y);
        a[0].color = sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
        a[1].position = sf::Vector2f(p2.x, p2.y);
        a[1].color = sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
        renderTarget->draw(a);
	}

	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf)
	{
        sf::VertexArray a(sf::Lines, 4);
        a[0].position = sf::Vector2f(xf.p.x, xf.p.y);
        a[1].position = sf::Vector2f(xf.p.x + xf.q.GetXAxis().x * 10, xf.p.y + xf.q.GetXAxis().y * 10);
        a[0].position = sf::Vector2f(xf.p.x, xf.p.y);
        a[1].position = sf::Vector2f(xf.p.x + xf.q.GetYAxis().x * 10, xf.p.y + xf.q.GetYAxis().y * 10);
        renderTarget->draw(a);
	}
};
b2World* CollisionManager::world;

void CollisionManager::initialize()
{
    world = new b2World(b2Vec2(0, 0));
#ifdef DEBUG
    new CollisionDebugDraw();
#endif
}

void CollisionManager::handleCollisions(float delta)
{
    world->Step(delta, 2, 4);
    for(b2Contact* contact = world->GetContactList(); contact; contact = contact->GetNext())
    {
        if (contact->IsTouching() && contact->IsEnabled())
        {
            Collisionable* A = (Collisionable*)contact->GetFixtureA()->GetBody()->GetUserData();
            Collisionable* B = (Collisionable*)contact->GetFixtureB()->GetBody()->GetUserData();
            if (!A->isDestroyed() && !B->isDestroyed())
            {
                A->collision(B);
                B->collision(A);
            }
        }
    }
}

Collisionable::Collisionable(float radius)
{
    position = sf::Vector2f(0, 0);
    rotation = 0.0;
    body = NULL;
    
    setCollisionRadius(radius);
}

Collisionable::Collisionable(sf::Vector2f boxSize, sf::Vector2f boxOrigin)
{
    position = sf::Vector2f(0, 0);
    rotation = 0.0;
    
    body = NULL;
    
    setCollisionBox(boxSize, boxOrigin);
}

Collisionable::~Collisionable()
{
    if (body)
        CollisionManager::world->DestroyBody(body);
}

void Collisionable::setCollisionRadius(float radius)
{
    b2CircleShape shape;
    shape.m_radius = radius;

    createBody(&shape);
}

void Collisionable::setCollisionBox(sf::Vector2f boxSize, sf::Vector2f boxOrigin)
{
    b2PolygonShape shape;
    shape.SetAsBox(boxSize.x / 2.0, boxSize.y / 2.0, b2Vec2(boxOrigin.x, boxOrigin.y), 0);

    createBody(&shape);
}

void Collisionable::setCollisionShape(std::vector<sf::Vector2f> shapeList)
{
    b2PolygonShape shape;
    shape.m_count = shapeList.size();
    for(unsigned int n=0; n<shapeList.size(); n++)
        shape.m_vertices[n] = b2Vec2(shapeList[n].x, shapeList[n].y);
    
    createBody(&shape);
}

void Collisionable::createBody(b2Shape* shape)
{
    if (body)
        CollisionManager::world->DestroyBody(body);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.userData = this;
    bodyDef.allowSleep = false;
    body = CollisionManager::world->CreateBody(&bodyDef);
    
    b2FixtureDef shapeDef;
    shapeDef.shape = shape;
    shapeDef.density = 1.0;
    shapeDef.isSensor = true;
    body->CreateFixture(&shapeDef);
}

void Collisionable::collision(Collisionable* target)
{
}

void Collisionable::setPosition(sf::Vector2f v)
{
    position = v;
    body->SetTransform(b2Vec2(position.x, position.y), rotation / 180.0 * M_PI);
}
sf::Vector2f Collisionable::getPosition()
{
    return position;
}

void Collisionable::setRotation(float angle)
{
    rotation = angle;
    body->SetTransform(b2Vec2(position.x, position.y), rotation / 180.0 * M_PI);
}

float Collisionable::getRotation()
{
    return rotation;
}
