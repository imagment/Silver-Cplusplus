#include "Silver.hpp"

int main() {
    Actor c1;
    c1.AddComponent<Camera>();

    Actor actor("alert", "This is your first window\n<red>SILVER!</red>");
    
    actor.GetComponent<Transform>()->position = Vector3Zero;
    //actor.GetComponent<SpriteRenderer>()->alignShapeTo(1.0f);

    actor.AddObject();

    c1.GetComponent<Camera>()->StartVideo();
    Hold();

    return 0;
}
