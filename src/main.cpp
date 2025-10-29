#include "GrammarBicol.h"
#include <iostream>

int main()
{
        GameEngine revivers("assets/gameVerbs.json", "assets/gameNouns.json");
        revivers.run();
}
