echo "Starting Compilation"
echo "Building math libraries..."
g++ -c ../thirdparty/gamemath/EulerAngles.cpp ../thirdparty/gamemath/MathUtil.cpp ../thirdparty/gamemath/RotationMatrix.cpp ../thirdparty/gamemath/Quaternion.cpp ../thirdparty/gamemath/Matrix4x3.cpp
echo "Linking math libraries to object..."
ld -r EulerAngles.o MathUtil.o RotationMatrix.o Quaternion.o Matrix4x3.o -o ../thirdparty/gamemath/gamemath.o
echo "Compiling source; linking to SFML, Artemis..."
g++ -std=c++11 -I../thirdparty/include -L../thirdparty/lib ../thirdparty/gamemath/gamemath.o ../src/Main.cpp ../src/MinimapSystem.cpp ../src/Components.cpp ../src/CameraSystem.cpp ../src/RenderSystem.cpp ../src/DebugSystems.cpp -lArtemisCpp -lsfml-graphics -lsfml-window -lsfml-system -o ../bin/GamePrototype.out
echo "Cleaning up directories..."
rm *.o
echo "Done."
