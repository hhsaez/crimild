#!/bin/bash
# Usage: sh build_shaders.sh [SHADERS_DIR]

#echo "Compiling shaders"
VULKAN_COMPILER=$VULKAN_SDK/bin/glslc

#echo "Encoding resources"
CRIMILD_RESOURCE_ENCODER=$CRIMILD_HOME/tools/resourceEncoder/resourceEncoder

WORK_DIR=$1
pushd $WORK_DIR > /dev/null

for file in `ls ./*.{vert,frag}`; do
	echo "Compiling $file"
	$VULKAN_COMPILER $file -o "$file.spv"
	
	echo "Encoding $file"
	$CRIMILD_RESOURCE_ENCODER $file "$file.cpp"
done

popd > /dev/null

echo "Done"
