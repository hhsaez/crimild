#!/bin/bash
echo "Compiling shaders"
VULKAN_COMPILER=$VULKAN_SDK/bin/glslc
$VULKAN_COMPILER $1.vert -o $1.vert.spv
$VULKAN_COMPILER $1.frag -o $1.frag.spv

echo "Encoding resources"
CRIMILD_RESOURCE_ENCODER=../../../../../tools/resourceEncoder/resourceEncoder
$CRIMILD_RESOURCE_ENCODER $1.vert.spv $1.vert.cpp
$CRIMILD_RESOURCE_ENCODER $1.frag.spv $1.frag.cpp

echo "Done"
