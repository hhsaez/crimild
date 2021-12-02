require 'assets/prefabs/fire'
require 'assets/prefabs/animated_fire'
require 'assets/prefabs/animated_smoke'
require 'assets/prefabs/sprinklers'
require 'assets/prefabs/explosion'
require 'assets/prefabs/fountain'
require 'assets/prefabs/smoke'
require 'assets/prefabs/flowers'
require 'assets/prefabs/attractors'
require 'assets/prefabs/sparkles'
require 'assets/prefabs/walkers'
require 'assets/prefabs/handsOnFire'

local camera = {
	type = 'crimild::Camera',
	enableCulling = false,
	transformation = {
		translate = { 0.0, 10.0, 20.0 },
	},
	components = {
		{
			type = 'crimild::FreeLookCameraComponent',
		},
	},
}

local environment = {
    type = 'crimild::Group',
    nodes = {
        {
            sceneFileName = 'assets/models/room.obj',
        },
        {
            type = 'crimild::Light',
            lightType = 'point',
            color = { 1.0, 1.0, 1.0, 1.0 },
            transformation = { translate = { 0.0, 1.0, 0.0 } },
        },
    },
}

scene = {
	type = 'crimild::Group',
	nodes = {
		environment,
		camera,

		animated_fire( -10.0, 0.5, 0.0 ),
		fire( -10.0, 0.5, -10.0 ),
		flowers( -10.0, 1.0, -20.0 ),
		sprinklers( -10.0, 2.0, -20.0 ),
		explosion( -10.0, 0.0, -60.0 ),
		attractors( -10.0, 5.0, -40.0 ),

		animated_smoke( 10.0, 0.5, 0.0 ),
		fountain( 10.0, 0.5, -10.0 ),
		smoke( 5.0, 5.0, -50.0, false ),
		smoke( 15.0, 5.0, -50.0, true ),
		sparkles( 15.0, 10.0, -30.0 ),

		walkers( 0.0, 0.0, -40.0 ),
		handsOnFire( 0.0, 0.0, -60.0 ),
	},
}
