function fountain( x, y, z )
	
	local MAX_PARTICLES = 500
	
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
				preWarmTime = 2.5,
				generators = {
					{
						type = 'crimild::BoxPositionParticleGenerator',
						origin = { 0.0, 0.0, 0.0 },
						size = { 5.0, 0.1, 5.0 },
					},
					{
						type = 'crimild::RandomVector3fParticleGenerator',
						attrib = 'velocity',
						minValue = { 0.0, 1.0, 0.0 },
						maxValue = { 0.0, 10.0, 0.0 },
					},
					{
						type = 'crimild::DefaultVector3fParticleGenerator',
						attrib = 'acceleration',
						value = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::ColorParticleGenerator',
						minStartColor = { 0.7, 0.0, 0.7, 1.0 },
						maxStartColor = { 1.0, 1.0, 1.0, 1.0 },
						minEndColor = { 0.5, 0.0, 0.6, 0.0 },
						maxEndColor = { 0.7, 0.5, 0.1, 0.0 },
					},
					{
						type = 'crimild::RandomReal32ParticleGenerator',
						attrib = 'uniformScale',
						minValue = 5.0,
						maxValue = 50.0,
					},
					{
						type = 'crimild::TimeParticleGenerator',
						minTime = 1.0,
						maxTime = 5.0,
					},
				},
				updaters = {
					{
						type = 'crimild::EulerParticleUpdater',
						globalAcceleration = { 0.0, -10.0, 0.0 },
					},
					{
						type = 'crimild::TimeParticleUpdater',
					},
					{
						type = 'crimild::FloorParticleUpdater',
					},
				},
				renderers = {
					{
						type = 'crimild::PointSpriteParticleRenderer',
					},
				},
			},
		},
		transformation = {
			translate = { x, y, z },
		},
	}
end


