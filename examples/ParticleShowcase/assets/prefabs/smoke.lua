function smoke( x, y, z, computeInWorldSpace )
	
	local MAX_PARTICLES = 200
	
	local ps = {
		type = 'crimild::Group',
		components = {
			{
				type = 'crimild::ParticleSystemComponent',
				particles = {
					type = 'crimild::ParticleData',
					maxParticles = MAX_PARTICLES,
					computeInWorldSpace = computeInWorldSpace,
				},
				emitRate = 0.25 * MAX_PARTICLES,
				generators = {
					{
						type = 'crimild::BoxPositionParticleGenerator',
						origin = { 0.0, 0.0, 0.0 },
						size = { 0.05, 0.05, 0.05 },
					},
					{
						type = 'crimild::RandomVector3fParticleGenerator',
						attrib = 'velocity',
						minValue = { 0.0, 1.0, 0.0 },
						maxValue = { 0.0, 2.0, 0.0 },
					},
					{
						type = 'crimild::DefaultVector3fParticleGenerator',
						attrib = 'acceleration',
						value = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::ColorParticleGenerator',
						minStartColor = { 1.0, 1.0, 1.0, 1.0 },
						maxStartColor = { 1.0, 1.0, 1.0, 1.0 },
						minEndColor = { 1.0, 1.0, 1.0, 0.0 },
						maxEndColor = { 1.0, 1.0, 1.0, 0.0 },
					},
					{
						type = 'crimild::RandomReal32ParticleGenerator',
						attrib = 'uniformScale',
						minValue = 20.0,
						maxValue = 100.0,
					},
					{
						type = 'crimild::TimeParticleGenerator',
						minTime = 1.0,
						maxTime = 2.0,
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
						type = 'crimild::PointSpriteParticleRenderer',
						material = {
							type = 'crimild::Material',
							colorMap = {
								type = 'crimild::Texture',
								image = {
									type = 'crimild::ImageTGA',
									imageFileName = 'assets/textures/smoke.tga',
								},
							},
						},
						blendMode = 'additive',
						cullFaceEnabled = false,
						depthStateEnabled = false,
					},
				},
			},
			{
				type = 'crimild::OrbitComponent',
				x0 = 0.0,
				y0 = 0.0,
				major = 5.0,
				minor = 3.0,
				speed = 2.0,
				gamma = 1.0,
			},
		},
	}

	return {
		type = 'crimild::Group',
		nodes = {
			ps,
		},
		transformation = {
			translate = { x, y, z },
		},
	}
end

