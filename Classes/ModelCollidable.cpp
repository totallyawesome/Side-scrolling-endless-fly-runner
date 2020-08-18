//
//  ModelCollidable.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelCollidable.h"

NS_RIY_BEGIN

b2Vec2 ModelCollidable::getOriginalHalfWidthAndHeightForBody(b2Body *body){
    bool first = true;
    float leftMost = 0.0f;
    float rightMost = 0.0f;
    float topMost = 0.0f;
    float bottomMost = 0.0f;
    b2Vec2 result;

    for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
    {
        b2Shape::Type shapeType = f->GetType();
        if ( shapeType == b2Shape::e_circle )
        {
            b2CircleShape *circleShape = (b2CircleShape*)f->GetShape();
            float tempRight = circleShape->m_p.x + circleShape->m_radius;
            float tempLeft = circleShape->m_p.x - circleShape->m_radius;
            float tempTop = circleShape->m_p.y + circleShape->m_radius;
            float tempBottom = circleShape->m_p.y - circleShape->m_radius;

            if (first) {
                first = false;

                rightMost = tempRight;
                leftMost = tempLeft;
                topMost = tempTop;
                bottomMost = tempBottom;
            }else{
                if (tempRight > rightMost) {
                    rightMost = tempRight;
                }

                if (tempLeft < leftMost) {
                    leftMost = tempLeft;
                }

                if (tempTop > topMost) {
                    topMost = tempTop;
                }

                if (tempBottom < bottomMost) {
                    bottomMost = tempBottom;
                }
            }
        }
        else if ( shapeType == b2Shape::e_polygon )
        {
            b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
            int i = 0;
            int count = polygonShape->GetVertexCount();
            for (i = 0; i<count; i++){
                b2Vec2 vertex = polygonShape->GetVertex(i);
                if (first) {
                    leftMost = vertex.x;
                    rightMost = vertex.x;
                    topMost = vertex.y;
                    bottomMost = vertex.y;
                    first = false;
                }else{
                    if(vertex.x < leftMost){
                        leftMost = vertex.x;
                    }

                    if(vertex.x > rightMost){
                        rightMost = vertex.x;
                    }

                    if(vertex.y > topMost){
                        topMost = vertex.y;
                    }

                    if (vertex.y < bottomMost) {
                        bottomMost = vertex.y;
                    }
                }
            }
        }
    }

    result = b2Vec2((rightMost - leftMost)/2,(topMost - bottomMost)/2);

    return result;
}

NS_RIY_END
