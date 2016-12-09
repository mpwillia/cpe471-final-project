
build="./build"
if [[ ! -d $build ]]; then
   mkdir $build
fi
cd $build

if [[ ! -f "Makefile" ]]; then
   echo "Creating Makefile"
   cmake ..
fi

echo "Building..."
make -j

if [ $? -eq 0 ]; then
   echo "Built Project Successfully"
   echo "=== Running Project ==="
   ./final_project ../resources/
fi


