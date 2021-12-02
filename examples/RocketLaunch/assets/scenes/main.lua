require 'assets/prefabs/vapor'
require 'assets/prefabs/fire_exhaust'

local captions = {
	type = 'crimild::Text',
	font = 'assets/fonts/Verdana',
	textSize = 0.05,
	text = ' ',
	textColor = { 1.0, 1.0, 0.0, 1.0 },
	textAnchor = 'center',
	enableDepthTest = false,
	transformation = {
		translate = { 0.0, -0.35, 0.0 },
	},
	components = {
		{
			type = 'crimild::demos::CaptionsController',
			lines = {
				{ text = '10', from = 1.0, to = 1.75, },
				{ text = '9', from = 2.0, to = 2.75, },
				{ text = '8', from = 3.0, to = 3.75, },
				{ text = '7', from = 4.0, to = 4.75, },
				{ text = '6', from = 5.0, to = 5.75, },
				{ text = '5', from = 6.0, to = 6.75, },
				{ text = '4', from = 7.0, to = 7.75, },
				{ text = '3', from = 8.0, to = 8.75, },
				{ text = '2', from = 9.0, to = 9.75, },
				{ text = '1', from = 10.0, to = 10.75, },
				{ text = 'LAUNCH', from = 11.0, to = 13.75, },
			},
		},
	},
}

local profilerOutput = {
	type = 'crimild::Text',
	font = 'assets/fonts/Courier New',
	textSize = 0.015,
	text = ' ',
	textColor = { 1.0, 1.0, 1.0, 1.0 },
	enableDepthTest = false,
	transformation = {
		translate = { -0.5, 0.35, 0.0 },
	},
	components = {
		{
			type = 'crimild::demos::TextProfilerOutputHandler',
		},
	},
}

local camera = {
	type = 'crimild::Camera',
	enableCulling = false,
	transformation = {
		translate = { 30.0, 1.0, 20.0 },
		rotate_euler = { 10.0, 30.0, 0.0 },
	},
	nodes = {
		{
			type = 'crimild::Group',
			nodes = {
				captions,
				profilerOutput,
			},
			transformation = {
				translate = { 0.0, 0.0, -1.0 },
			},
		},
	},
	components = {
		{
			type = 'crimild::FreeLookCameraComponent',
		},
	},
}

local environment = {
	sceneFileName = 'assets/models/room.obj',
}

local rocket = {
	type = 'crimild::Group',
	nodes = {
		{ sceneFileName = 'assets/models/rocket.obj', },
		buildVaporFX( { translate = { 0.0, -2.0, 0.0 } } ),
		
		buildFireExhaustFX( { translate = { 0.0, -0.85, 0.0 } } ),
		buildFireExhaustFX( { translate = { -1.0, -0.85, 0.0 } } ),
		buildFireExhaustFX( { translate = { 1.0, -0.85, 0.0 } } ),
	},
	components = {
		{
			type = 'crimild::demos::RocketController',
			countdown = 5,
			ignition = 8,
			launch = 11,
			speed = 5.0,
		},
	},
	transformation = {
		translate = { 0.0, 2.0, -30.0 },
	},
}

local light = {
	type = 'crimild::Light',
	lightType = 'directional',
	transformation = {
		rotate_euler = { -45.0, 45.0, 0.0 },
	},
}

scene = {
	type = 'crimild::Group',
	nodes = {
		environment,
		camera,
		light,
		rocket,
	},
}
