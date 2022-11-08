rmdir /S /Q build
mkdir build
g++ -c source\ImageSaver.cpp -o build\ImageSaver.o
g++ -c source\Main.cpp -o build\Main.o
g++ -c source\Renderer.cpp -o build\Renderer.o
g++ -c source\Shapes.cpp -o build\Shapes.o
g++ -c source\Vector.cpp -o build\Vector.o
g++ build\* -o 3DRenderer
rmdir /S /Q build
