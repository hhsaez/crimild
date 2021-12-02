paddle = {
	filename = 'assets/models/paddle.obj',
	transformation = {
		translate = { 0.0, 0.0, 0.0 }
	},
	components = {
		{
			type = 'crimild::physics::RigidBody',
			mass = 0.0,
			convex = true,
			kinematic = true
		},
		{
			type = 'crimild::bricks::Paddle',
			speed = 0.5,
		}
	}
}

ball = {
	filename = 'assets/models/ball.obj',
	transformation = {
		translate = { 0.0, 2.0, 0.0 }
	},
	components = {
		{
			type = 'crimild::physics::RigidBody',
			mass = 1.0,
			convex = true,
			linearVelocity = { 15.0, 20.0, 0.0 },
			linearFactor = { 1.0, 1.0, 0.0 },
			angularFactor = { 0.0, 0.0, 1.0 },
			constraintVelocity = true,
		},
		{
			type = 'crimild::bricks::Ball',
		}
	}
}

border = {
	filename = 'assets/models/border.obj',
	transformation = {
		translate = { 0.0, 0.0, 0.0 }
	},
	components = {
		{
			type = 'crimild::physics::RigidBody',
			mass = 0.0,
			convex = false,
			kinematic = false
		}
	}
}

bricks = {
	type = 'crimild::Group',
	nodes = {
		{
			filename = 'assets/models/brick.obj',
			transformation = {
				translate = { 5.0, 0.0, 0.0 }
			},	
			components = {
				{
					type = 'crimild::physics::RigidBody',
					mass = 0.0,
					convex = true,
					kinematic = true
				},			
			}
		}
	}
}

for x = 0, 6 do
	for y = 0, 5 do
		table.insert(bricks.nodes, {
			filename = 'assets/models/brick.obj',
			transformation = {
				translate = { -9.0 + ( x * 3.0 ), 20.0 + ( y * 2.0 ), 0.0 }
			},	
			components = {
				{
					type = 'crimild::physics::RigidBody',
					mass = 0.0,
					convex = true,
					kinematic = true
				},			
				{
					type = 'crimild::bricks::Brick',
				},
			}
		})
	end
end

camera = {
	type = 'crimild::Camera',
	transformation = {
		translate = { 0.0, 14.0, 50.0 },
		-- lookAt = { 0.0, 10.0, 0.0 },
	},
	nodes = {
		{
			filename = 'assets/models/background.obj',
			transformation = {
				translate = { 0.0, 0.0, -200.0 },
				scale = 100.0,
			}
		}
	},
}

light = {
	type = 'crimild::Light',
	transformation = {
		translate = { 0.0, 10.0, 10.0 },
	},
}

scene = {
	nodes = {
		border,
		paddle,
		ball,
		bricks,
		camera,
		light,
	},
	components = {
		{ type = 'crimild::bricks::GameController' },
	},
}

