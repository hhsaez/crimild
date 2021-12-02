function explosion( x, y, z )
	
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
				emitRate = MAX_PARTICLES,
				burst = true,
				generators = {
					{
						type = 'crimild::SpherePositionParticleGenerator',
						origin = { 0.0, 5.0, 0.0 },
						size = { 0.1, 0.1, 0.1 },
					},
					{
						type = 'crimild::SphereVelocityParticleGenerator',
						magnitude = { 5.0, 5.0, 5.0 },
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
						minTime = 3.0,
						maxTime = 3.0,
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


