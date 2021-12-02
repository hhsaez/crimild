function attractors( x, y, z )
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
				emitRate = MAX_PARTICLES,
				preWarmTime = 10.0,
				burst = true,
				generators = {
					{
						type = 'crimild::SpherePositionParticleGenerator',
						origin = { 0.0, 0.0, 0.0 },
						size = { 0.1, 0.1, 0.1 },
					},
					{
						type = 'crimild::DefaultVector3fParticleGenerator',
						attrib = 'velocity',
						value = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::DefaultVector3fParticleGenerator',
						attrib = 'acceleration',
						value = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::ColorParticleGenerator',
						minStartColor = { 0.0, 1.0, 0.0, 1.0 },
						maxStartColor = { 1.0, 1.0, 0.0, 1.0 },
						minEndColor = { 0.0, 0.75, 0.0, 0.0 },
						maxEndColor = { 1.5, 0.95, 0.0, 0.0 },
					},
					{
						type = 'crimild::RandomReal32ParticleGenerator',
						attrib = 'uniformScale',
						minValue = 10.0,
						maxValue = 20.0,
					},
					{
						type = 'crimild::TimeParticleGenerator',
						minTime = 30.0,
						maxTime = 30.0,
					},
				},
				updaters = {
					{
						type = 'crimild::SetVector3fValueParticleUpdater',
						attrib = 'acceleration',
						value = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::AttractorParticleUpdater',
						origin = { x, y + 3.0, z },
						radius = 5.0,
						strength = 50.0,
					},
					{
						type = 'crimild::AttractorParticleUpdater',
						origin = { x, y, z },
						radius = 2.0,
						strength = -25.0, --repulsor
					},
					{
						type = 'crimild::EulerParticleUpdater',
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


