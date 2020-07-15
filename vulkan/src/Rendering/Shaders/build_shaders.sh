#!/bin/bash
# Usage: sh build_shaders.sh [SHADERS_DIR]

if [ -z "$VULKAN_SDK" ]
then
	# Fallback fro $VULKAN_SDK
	VULKAN_SDK=~/Development/bin/vulkansdk-macos/macOS
fi

if [ -z "$CRIMILD_HOME" ]
then
	# Fallback for $CRIMILD_HOME
	CRIMILD_HOME=~/Development/Crimild/crimild
fi

#echo "Compiling shaders"
VULKAN_COMPILER=$VULKAN_SDK/bin/glslc

#echo "Encoding resources"
CRIMILD_RESOURCE_ENCODER=$CRIMILD_HOME/tools/resourceEncoder/resourceEncoder

WORK_DIR=$1
pushd $WORK_DIR > /dev/null

for file in `find . -type f \( -name "*.vert" -o -name "*.frag" \) -print`; do
	echo "Compiling $file"
	$VULKAN_COMPILER $file -o "$file.spv"

	echo "Encoding $file"
	$CRIMILD_RESOURCE_ENCODER "$file.spv" "$file.inl"
done

popd > /dev/null

echo "Done"
