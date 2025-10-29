echo "Building C..."
cd ./src/c/
./build.sh
cd ../..
echo "================================="

echo "Building C++..."
cd ./src/cpp/
./build.sh
cd ../..
echo "================================="

echo "Building C#..."
cd ./src/csharp/
./build.sh
cd ../..
echo "================================="

echo "Building Go..."
cd ./src/go/
./build.sh
cd ../..
echo "================================="

echo "Building Java..."
cd ./src/java/
./build.sh
cd ../..
echo "================================="

echo "Building Rust..."
cd ./src/rust/
./build.sh
cd ../..
echo "================================="
