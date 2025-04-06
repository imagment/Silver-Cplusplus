#include "Silver.hpp"

int main() {

	Actor c1;

	c1.AddComponent<Camera>();

	Actor actor("alert", "Hello World!");

	actor.GetComponent<Transform>()->position = Vector3Zero;

	actor.GetComponent<Transform>()->scale = Vector3(1,1,1);

	actor.AddObject();

	c1.GetComponent<Camera>()->RenderFrame();

	Hold();

	return 0;

}
