echo "Building C..."
cd ./src/c/
./build.ps1
cd ../..
echo "================================="

echo "Building C++..."
cd ./src/cpp/
./build.ps1
cd ../..
echo "================================="

echo "Building C#..."
cd ./src/csharp/
./build.ps1
cd ../..
echo "================================="

echo "Building Go..."
cd ./src/go/
./build.ps1
cd ../..
echo "================================="

echo "Building Java..."
cd ./src/java/
./build.ps1
cd ../..
echo "================================="

echo "Building Rust..."
cd ./src/rust/
./build.ps1
cd ../..
echo "================================="
