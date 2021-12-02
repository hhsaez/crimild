function flowers( x, y, z )

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
						type = 'crimild::BoxPositionParticleGenerator',
						origin = { 0.0, 0.0, 0.0 },
						size = { 5.0, 0.0, 5.0 },
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
						minValue = 1.0,
						maxValue = 3.0,
					},
				},
				updaters = {
					{
						type = 'crimild::CameraSortParticleUpdater',
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
									imageFileName = 'assets/textures/jasmine.tga',
								},
							},
						},
						blendMode = 'default',
						cullFaceEnabled = false,
					},
				},
			},
		},
		transformation = {
			translate = { x, y, z },
		},
	}
end


