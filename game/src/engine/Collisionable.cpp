#include "Collisionable.h"
#include "Renderable.h"
#include "vectorUtils.h"

#define BOX2D_SCALE 100.0f
static inline sf::Vector2f b2v(b2Vec2 v)
{
    return sf::Vector2f(v.x * BOX2D_SCALE, v.y * BOX2D_SCALE);
}
static inline b2Vec2 v2b(sf::Vector2f v)
{
    return b2Vec2(v.x / BOX2D_SCALE, v.y / BOX2D_SCALE);
}

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
            a[n].position = b2v(vertices[n]);
        a[vertexCount].position = b2v(vertices[0]);
        renderTarget->draw(a);
	}

	/// Draw a solid closed polygon provided in CCW order.
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
        sf::VertexArray a(sf::LinesStrip, vertexCount+1);
        for(int32 n=0; n<vertexCount; n++)
        {
            a[n].position = b2v(vertices[n]);
            a[n].color = sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
        }
        a[vertexCount].position = b2v(vertices[0]);
        renderTarget->draw(a);
	}

	/// Draw a circle.
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
	{
        sf::CircleShape shape(radius * BOX2D_SCALE, 16);
        shape.setOrigin(radius * BOX2D_SCALE, radius * BOX2D_SCALE);
        shape.setPosition(b2v(center));
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255));
        shape.setOutlineThickness(1.0);
        renderTarget->draw(shape);
	}
	
	/// Draw a solid circle.
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
	{
        sf::CircleShape shape(radius * BOX2D_SCALE, 16);
        shape.setOrigin(radius * BOX2D_SCALE, radius * BOX2D_SCALE);
        shape.setPosition(b2v(center));
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255));
        shape.setOutlineThickness(1.0);
        renderTarget->draw(shape);
	}
	
	/// Draw a line segment.
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
	{
        sf::VertexArray a(sf::Lines, 2);
        a[0].position = b2v(p1);
        a[0].color = sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
        a[1].position = b2v(p2);
        a[1].color = sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
        renderTarget->draw(a);
	}

	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf)
	{
        sf::VertexArray a(sf::Lines, 4);
        a[0].position = b2v(xf.p);
        a[1].position = b2v(xf.p) + sf::Vector2f(xf.q.GetXAxis().x * 10, xf.q.GetXAxis().y * 10);
        a[0].position = b2v(xf.p);
        a[1].position = b2v(xf.p) + sf::Vector2f(xf.q.GetYAxis().x * 10, xf.q.GetYAxis().y * 10);
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
    Collisionable* destroy = NULL;
    world->Step(delta, 4, 8);
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
            }else{
                if (A->isDestroyed())
                    destroy = A;
                if (B->isDestroyed())
                    destroy = B;
            }
        }
    }
    
    //Lazy cleanup of already destroyed bodies. We cannot destroy the bodies while we are walking trough the ContactList, as it would invalidate the contact we are iterating on.
    if (destroy)
    {
        world->DestroyBody(destroy->body);
        destroy->body = NULL;
    }
}

Collisionable::Collisionable(float radius)
{
    rotation = 0.0;
    enablePhysics = false;
    body = NULL;
    
    setCollisionRadius(radius);
}

Collisionable::Collisionable(sf::Vector2f boxSize, sf::Vector2f boxOrigin)
{
    rotation = 0.0;
    enablePhysics = false;
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
    shape.m_radius = radius / BOX2D_SCALE;

    createBody(&shape);
}

void Collisionable::setCollisionBox(sf::Vector2f boxSize, sf::Vector2f boxOrigin)
{
    b2PolygonShape shape;
    shape.SetAsBox(boxSize.x / 2.0 / BOX2D_SCALE, boxSize.y / 2.0 / BOX2D_SCALE, v2b(boxOrigin), 0);

    createBody(&shape);
}

void Collisionable::setCollisionShape(std::vector<sf::Vector2f> shapeList)
{
    b2PolygonShape shape;
    shape.m_count = shapeList.size();
    for(unsigned int n=0; n<shapeList.size(); n++)
        shape.m_vertices[n] = v2b(shapeList[n]);
    
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
    shapeDef.isSensor = !enablePhysics;
    body->CreateFixture(&shapeDef);
}

void Collisionable::collision(Collisionable* target)
{
}

void Collisionable::setPosition(sf::Vector2f position)
{
    body->SetTransform(v2b(position), rotation / 180.0 * M_PI);
}

sf::Vector2f Collisionable::getPosition()
{
    return b2v(body->GetPosition());
}

void Collisionable::setRotation(float angle)
{
    rotation = angle;
    body->SetTransform(body->GetPosition(), rotation / 180.0 * M_PI);
}

float Collisionable::getRotation()
{
    return body->GetAngle() / M_PI * 180.0;
}

void Collisionable::setVelocity(sf::Vector2f velocity)
{
    body->SetLinearVelocity(v2b(velocity));
}
sf::Vector2f Collisionable::getVelocity()
{
    return b2v(body->GetLinearVelocity());
}
