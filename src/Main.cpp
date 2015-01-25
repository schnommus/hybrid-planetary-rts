#include "Game.h"

int main(int argc, char **argv) {
    Game g;
	g.Initialize();
	g.LoadLevel();
	//g.EnableLevelEditor();
	g.Run();
    return 0;
}
