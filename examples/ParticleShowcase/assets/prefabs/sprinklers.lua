function sprinklers( x, y, z )
	
	local MAX_PARTICLES = 500
	
	return {
		type = 'crimild::Group',
		components = {
			{
				type = 'crimild::ParticleSystemComponent',
				particles = {
					type = 'crimild::ParticleData',
					maxParticles = MAX_PARTICLES,
					computeInWorldSpace = true,
				},
				emitRate = 0.25 * MAX_PARTICLES,
				generators = {
					{
						type = 'crimild::BoxPositionParticleGenerator',
						origin = { 0.0, 0.0, 0.0 },
						size = { 0.5, 0.5, 0.5 },
					},
					{
						type = 'crimild::RandomVector3fParticleGenerator',
						attrib = 'velocity',
						minValue = { -3.0, 5.0, -3.0 },
						maxValue = { 3.0, 8.0, 3.0 },
					},
					{
						type = 'crimild::DefaultVector3fParticleGenerator',
						attrib = 'acceleration',
						value = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::ColorParticleGenerator',
						minStartColor = { 0.0, 0.0, 0.7, 1.0 },
						maxStartColor = { 1.0, 1.0, 1.0, 1.0 },
						minEndColor = { 0.0, 0.0, 0.25, 0.0 },
						maxEndColor = { 0.0, 0.0, 0.7, 0.0 },
					},
					{
						type = 'crimild::RandomReal32ParticleGenerator',
						attrib = 'uniformScale',
						minValue = 10.0,
						maxValue = 20.0,
					},
					{
						type = 'crimild::TimeParticleGenerator',
						minTime = 1.0,
						maxTime = 1.5,
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


