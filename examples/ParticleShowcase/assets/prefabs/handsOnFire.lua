function handsOnFire( x, y, z )

	local MAX_PARTICLES = 200

	local astroboy = {
		sceneFileName = 'assets/models/astroboy/astroBoy_walk_Max.dae',
		transformation = {
			scale = 70.0,
		},
	}

	local ps = {
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
						type = 'crimild::NodePositionParticleGenerator',
						node = 'R_middle_01',
						size = { 0.25, 0.25, 0.25 },
					},
					{
						type = 'crimild::RandomVector3fParticleGenerator',
						attrib = 'velocity',
						minValue = { 0.0, 1.0, 0.0 },
						maxValue = { 0.0, 2.5, 0.0 },
					},
					{
						type = 'crimild::DefaultVector3fParticleGenerator',
						attrib = 'acceleration',
						value = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::RandomReal32ParticleGenerator',
						attrib = 'uniformScale',
						minValue = 0.25,
						maxValue = 0.75,
					},
					{
						type = 'crimild::TimeParticleGenerator',
						minTime = 0.25,
						maxTime = 0.75,
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
						type = 'crimild::OrientedQuadParticleRenderer',
						material = {
							type = 'crimild::Material',
							colorMap = {
								type = 'crimild::Texture',
								image = {
									type = 'crimild::ImageTGA',
									imageFileName = 'assets/textures/fire.tga',
								},
							},
						},
						blendMode = 'additive',
						cullFaceEnabled = false,
						depthStateEnabled = false,
					},
				},
			},
		},
	}

	return {
		type = 'crimild::Group',
		nodes = {
			astroboy,
			ps,
		},
		transformation = {
			translate = { x, y, z },
		},
	}
end

