#include "Silver.hpp"

int main() {
    Actor c1;
    c1.AddComponent<Camera>();

    Actor actor("alert");

    actor.GetComponent<Transform>()->scale = Vector3Zero;
    actor.AddComponent<SpriteRenderer>()->setShape("This is your first window!");

    actor.AddObject();

    c1.GetComponent<Camera>()->RenderFrame();

    return 0;
}
