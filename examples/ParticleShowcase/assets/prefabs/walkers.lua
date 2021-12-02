function walkers( x, y, z )

	local MAX_PARTICLES = 10

	local nodes = {}
	for i = 1, MAX_PARTICLES do
		table.insert(
			nodes,
			{
				sceneFileName = 'assets/models/astroboy/astroBoy_walk_Max.dae',
				transformation = {
					scale = 15.0,
				},
			}
		)
	end
	
	return {
		type = 'crimild::Group',
		components = {
			{
				type = 'crimild::ParticleSystemComponent',
				particles = {
					type = 'crimild::ParticleData',
					maxParticles = MAX_PARTICLES,
				},
				emitRate = 0.75 * MAX_PARTICLES,
				generators = {
					{
						type = 'crimild::BoxPositionParticleGenerator',
						origin = { 0.0, 0.0, 0.0 },
						size = { 5.0, 0.0, 5.0 },
					},
					{
						type = 'crimild::RandomVector3fParticleGenerator',
						attrib = 'velocity',
						minValue = { -2.0, 0.0, -2.0 },
						maxValue = { 2.0, 0.0, 2.0 },
					},
					{
						type = 'crimild::DefaultVector3fParticleGenerator',
						attrib = 'acceleration',
						value = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::TimeParticleGenerator',
						minTime = 10.0,
						maxTime = 15.0,
					},
				},
				updaters = {
					{
						type = 'crimild::EulerParticleUpdater',
					},
					{
						type = 'crimild::TimeParticleUpdater',
					},
				},
				renderers = {
					{
						type = 'crimild::NodeParticleRenderer',
					},
				},
			},
		},
		transformation = {
			translate = { x, y, z },
		},
		nodes = nodes,
	}
end


