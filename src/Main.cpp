#include "Game.h"

int main(int argc, char **argv) {
    Game g;
	g.Initialize();
	//g.GenerateLevel();
	g.LoadLevel();
	//g.EnableLevelEditor();
	g.Run();
	g.SaveLevel();
    return 0;
}
