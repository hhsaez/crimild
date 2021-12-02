function buildFireExhaustFX( transformation )
	
	local MAX_PARTICLES = 300
	
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
				emitRate = 0.5 * MAX_PARTICLES,
				generators = {
					{
						type = 'crimild::BoxPositionParticleGenerator',
						origin = { 0.0, 0.0, 0.0 },
						size = { 0.5, 0.0, 0.5 },
					},
					{
						type = 'crimild::RandomVector3fParticleGenerator',
						attrib = 'velocity',
						minValue = { 0.0, -3.0, 0.0 },
						maxValue = { 2.0, -1.0, 2.0 },
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
						minEndColor = { 1.0, 0.0, 0.0, 0.0 },
						maxEndColor = { 1.0, 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::RandomReal32ParticleGenerator',
						attrib = 'uniformScale',
						minValue = 0.1,
						maxValue = 1.0,
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
					},
					{
						type = 'crimild::TimeParticleUpdater',
					},
					{
						type = 'crimild::FloorParticleUpdater',
					},
					{
						type = 'crimild::CameraSortParticleUpdater',
					},
					{
						type = 'crimild::ColorParticleUpdater',
					},
				},
				renderers = {
					{
						type = 'crimild::AnimatedSpriteParticleRenderer',
						spriteSheetSize = { 4.0, 4.0 },
						material = {
							type = 'crimild::Material',
							colorMap = {
								type = 'crimild::Texture',
								image = {
									type = 'crimild::ImageTGA',
									imageFileName = 'assets/textures/flames.tga',
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
		transformation = transformation,
	}
end


