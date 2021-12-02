# Crimild Demos and Examples

## Table of Contents
+ [Basics](#Basics)
+ [Primitives](#Primitives)
+ [Textures](#Textures)
+ [Advanced](#Advanced)

## Basics
&nbsp; | &nbsp; | &nbsp;
-- | -- | --
<img src="Triangle/screenshot.png" width=200 />|[Triangle](Triangle/) | Basic (and very explicit) example for rendering a single colored triangle using Crimild. This shows the most important concepts and tools in the engine like geometries, buffers, pipelines and render passes.
<img src="Transformations/screenshot.png" width=200 /> | [Transformations](Transformations) | Creates a scene with many objects, each of them with a different transformations. Shows how to specify translations, rotations and scales using the local transformation of nodes.

## Primitives
&nbsp; | &nbsp; | &nbsp;
-- | -- | --
<img src="Points/screenshot.png" width=200 /> | [Points](Points) |Shows how to use a POINTS primitive to build a point cloud.
<img src="Lines/screenshot.png" width=200 /> | [Lines](Lines) | Shows how to use a LINES primitive.
<img src="Spheres/screenshot.png" width=200 /> | [Spheres](Spheres) | Shows how to use a Sphere primitive.
<img src="Boxes/screenshot.png" width=200 /> | [Boxes](Boxes) |Shows how to use a Box primitive.
<img src="ParametricPrimitives/screenshot.png" width=200 /> | [Parametric Primitives](ParametricPrimitives) | Shows how to use each of the parametric primitives, using different generator params, to construct several objects objects.
<a name="Textures">Textures</a> | |
<img src="Textures/screenshot.png" width=200 /> | [Textures](Textures/) | Loads an image from disk and create an image view, a sampler and a texture to display it on screen. Shows how to use primitives with texture coordinates as well.
<img src="TextureMipmaps/screenshot.png" width=200 /> | [Texture Mipmaps Generation](TextureMipmaps/) | Automatically generate mipmaps in runtime for a texture.
<img src="TextureFilters/screenshot.png" width=200 /> | [Texture Filters](TextureFilters/) | Create textures by using different min/mag filter options in samplers.
<img src="TextureWrapping/screenshot.png" width="200" /> | [Texture Wrap Mode](TextureWrapping/) | Create textures by using different wrap modes options in samplers.
<a name="Environment">Environment</a> | |
<img src="Skybox/screenshot.png" width="200" /> | [Skybox](Skybox) | Use a cubemap texture to render a skybox around a scene.
<img src="EnvironmentMapping/screenshot.png" width="200" /> | [Environment Mapping](EnviornmentMapping) | Implements simple reflection and refraction effects using environment mapping.
<a name="Lighting">Lighting</a> | |
<img src="LightingUnlit/screenshot.png" width="200" /> | [Lighting Unlit](LightingUnlit) | Uses an unlit material to show how colors are displayed when no lighing is available.
<img src="LightingMaterial/screenshot.png" width="200" /> | [Lighting Material](LightingMaterial) | Renders a scene with several objects using the LitMaterial class.
<img src="LightingDiffuseMap/screenshot.png" width="200" /> | [Lighting Diffuse Map](LightingDiffuseMap) | Renders Planet Earth with diffuse texture and single point light, using Phong shading model.
<img src="LightingDirectional/screenshot.png" width="200" /> | [Lighting Directional](LightingDirectional) | Uses a single directional light to lit a scene.
<img src="LightingPoint/screenshot.png" width="200" /> | [Lighting Point](LightingPoint) | Uses a single point light to lit a scene.
<img src="LightingSpotlight/screenshot.png" width="200" /> | [Lighting Spotlight](LightingSpotlight) | Uses a single spotlight light to lit a scene
<img src="LightingMulitple/screenshot.png" width="200" /> | [Lighting Multiple](LightingMultiple) | Creates a scene with lots of cubes and adds multiple light sources: one directional light, two point lights moving around the scene and a flashlight (using a spot light). Camera can be controlled with WASD and the mouse.
<img src="NormalMapping/screenshot.png" width="200" /> | [Normal Mapping](NormalMapping) | Uses a normal texture to provide more detail to models
<a name="Loaders">Loaders</a> | |
<img src="OBJLoader/screenshot.png" width="200" /> | [OBJLoader](OBJLoader) | Shows how to use the OBJ loader to create a simple scene by loading models from files.
<a name="DepthStencil">Depth/Stencil</a> | |
<img src="Depth/screenshot.png" width="200" /> | [Depth](Depth) | Visualizes the depth buffer by implementing a custom shader.
<img src="DepthFunc/screenshot.png" width="200" /> | [Depth Functions](DepthFunc) | Visualizes what happens when depth testing is disabled.
<img src="StencilOutline/screenshot.png" width="200" /> | [Stencil Outline](StencilOutline) | Renders an outline around some objects using the stencil buffer and multiple passes
<a name="Alpha">Alpha</a> | |
<img src="AlphaDiscard/screenshot.png" width="200" /> | [AlphaDiscard](AlphaDiscard) | Displays a scene with several objects, discarding fragments if the alpha value is lower than some threshold.
<a name="Pipelines">Pipelines</a> | |
<img src="PipelineCullMode/screenshot.png" width="200" /> | [Pipeline Cull mode](PipelineCullMode) | Modify a pipeline to disable back-face culling for a rotating quad.
<a name="Compositions">Compositions</a> | |
<img src="PostprocessingNegative/screenshot.png" width="200" /> | [Postprocessing Negative](PostprocessingNegative) | Applies a "negative" post processing effect by using frame compositions
<img src="PostprocessingGrayscale/screenshot.png" width="200" /> | [Postprocessing Grayscale](PostprocessingGrayscale) |  Converts a color scene into grayscale by using frame compositions
<img src="PostprocessingSharpen/screenshot.png" width="200" /> | [Postprocessing Sharpen](PostprocessingSharpen) | Applies a sharpen convolution to a rendered scene by using frame compositions
<img src="PostprocessingBlur/screenshot.png" width="200" /> | [Postprocessing Blur](PostprocessingBlur) | Applies a blur convolution to a rendered scene by using frame compositions
<img src="PostprocessingEdges/screenshot.png" width="200" /> | [Postprocessing Edges](PostprocessingEdges) | Process a rendered scene, highlighting edges by using frame compositions
<a name="Shadows">Shadows</a> | |
<img src="Shadows/screenshot.png" width="200" /> | [Directional](Shadows) | A simple scene is rendered using a directional light that cast shadows on both dynamic and static objects.
<img src="ShadowsSpot/screenshot.png" width="200" /> | [Spot](ShadowsSpot) | A simple scene is rendered using a spot light that cast shadows on both dynamic and static objects
<img src="ShadowsPoint/screenshot.png" width="200" /> | [Point](ShadowsPoint) | A scene is rendered using a point light that casting shadows for all objects in all directions
<img src="ShadowsDirectionalMultiple/screenshot.png" width="200" /> | [Directional (many lights)](ShadowsDirectionalMultiple) | A simple scene is rendered using two directional lights, each one casting shadows on both dynamic and static objects.
<img src="ShadowsSpotMultiple/screenshot.png" width="200" /> | [Spot (many lights)](ShadowsSpotMultiple) | A simple scene is rendered using three spot lights with different colors, each one casting shadows on both dynamic and static objects
<img src="ShadowsPointMultiple/screenshot.png" width="200" /> | [Point (many lights)](ShadowsPointMultiple) | A scene is rendered using three point lights with different colors, each one casting shadows for all objects in all directions
<a name="Image Effects">Image Effects</a> | |
<img src="HDR/screenshot.png" width="200" /> | [HDR](HDR) | Renders a scene with HDR enabled, providing more control over  bright and dark colors
<img src="Bloom/screenshot.png" width="200" /> | [Bloom](Bloom) | Filters brigth areas, applying blur to the result, to generate a light bleeding effect
<a name="Shaders">Shaders</a> | |
<img src="Smiley/screenshot.png" width="200" /> | [Smiley](Smiley) | TBD
<a name="PBR">PBR</a> | |
<img src="PBRBasic/screenshot.png" width="200" /> | [PBR Basic](PBRBasic) | TBD
<img src="PBRTexture/screenshot.png" width="200" /> | [PBR Texture](PBRTexture) | TBD
<img src="PBRIBL/screenshot.png" width="200" /> | [PBR Image Based Lighting](PBRIBL) | TBD
<img src="PBRModel/screenshot.png" width="200" /> | [PBR 3D Model](PBRModel) | TBD
<a name="Particles">Particles</a> | |
<img src="ParticlesFire/screenshot.png" width="200" /> | [Fire](ParticlesFire) | TBD
