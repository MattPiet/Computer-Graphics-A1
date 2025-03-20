#include "Collision.h"

bool Collision::SphereSphereCollisionDetected(const Body* body1, const Body* body2){
    float distance = VMath::distance(body1->pos, body2->pos);
    if (distance < body1->radius + body2->radius) {
        return true;
    }
    return false;
}

void Collision::SphereSphereCollisionResponse(Body* body1, Body* body2) {
    Vec3 normalVector = body1->pos - body2->pos;
 /*   if (VMath::mag(body2->vel) < VERY_SMALL) {
        return;
    }*/
    if (SphereSphereCollisionDetected(body1, body2) && VMath::dot(normalVector, VMath::normalize(normalVector)) > 0) {

        float relativeVelocity = VMath::dot(body1->vel - body2->vel, VMath::normalize(normalVector));
        float impulseJ = (-2.0f * relativeVelocity) / ((1 / body1->mass) + (1 / body2->mass));
        body2->pos = body1->pos - VMath::normalize(normalVector) * 2;
        body1->vel += (impulseJ * VMath::normalize(normalVector) / body1->mass);
        body2->vel -= (impulseJ * VMath::normalize(normalVector) / body2->mass);

        std::cout << "The vel of a non specific tenticle sphere is: ";  body2->vel.print();

      //  body1->angVel = VMath::cross(Vec3(0.0f, 1.0f, 0.0f), body1->vel);
       // body2->angVel = VMath::cross(Vec3(0.0f, 1.0f, 0.0f), body2->vel);
    }
}
