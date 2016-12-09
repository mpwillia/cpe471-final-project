
build="./build"
if [[ ! -f $build ]]; then
   mkdir $build
fi

cd $build
cmake ..
make -j

if [ $? -eq 0 ]; then

   echo "=== Running Project ==="
   
   ./final_project ../resources/

fi


