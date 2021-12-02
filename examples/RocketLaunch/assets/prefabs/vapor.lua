function buildVaporFX( transformation )
	
	local MAX_PARTICLES = 2000
	
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
				emitRate = 0.1 * MAX_PARTICLES,
				generators = {
					{
						type = 'crimild::BoxPositionParticleGenerator',
						origin = { 0.0, 0.0, 0.0 },
						size = { 1.0, 0.0, 1.0 },
					},
					{
						type = 'crimild::RandomVector3fParticleGenerator',
						attrib = 'velocity',
						minValue = { -2.0, -2.0, -0.1 },
						maxValue = { 2.0, 0.5, 0.1 },
					},
					{
						type = 'crimild::RandomVector3fParticleGenerator',
						attrib = 'acceleration',
						minValue = { 0.0, 0.0, 0.0 },
						maxValue = { 0.0, 0.0, 0.0 },
					},
					{
						type = 'crimild::ColorParticleGenerator',
						minStartColor = { 1.0, 1.0, 1.0, 1.0 },
						maxStartColor = { 1.0, 1.0, 1.0, 1.0 },
						minEndColor = { 0.0, 0.0, 0.0, 1.0 },
						maxEndColor = { 0.0, 0.0, 0.0, 1.0 },
					},
					{
						type = 'crimild::RandomReal32ParticleGenerator',
						attrib = 'uniformScaleStart',
						minValue = 0.1,
						maxValue = 1.0,
					},
					{
						type = 'crimild::RandomReal32ParticleGenerator',
						attrib = 'uniformScaleEnd',
						minValue = 1.0,
						maxValue = 8.0,
					},
					{
						type = 'crimild::TimeParticleGenerator',
						minTime = 5.0,
						maxTime = 20.0,
					},
				},
				updaters = {
					{
						type = 'crimild::SetVector3fValueParticleUpdater',
						attrib = 'acceleration',
						value = { 0.0, 0.0, 0.0 },
					},
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
						type = 'crimild::UniformScaleParticleUpdater',
					},
					{
						type = 'crimild::ColorParticleUpdater',
					},
					{
						type = 'crimild::AttractorParticleUpdater',
						origin = { 10.0, 3.0, -30.0 },
						radius = 20.0,
						strength = 5000.0,
					},
					{
						type = 'crimild::AttractorParticleUpdater',
						origin = { -10.0, 3.0, -30.0 },
						radius = 20.0,
						strength = 5000.0,
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
									imageFileName = 'assets/textures/animated_smoke_2.tga',
								},
							},
						},
						blendMode = 'color',
						cullFaceEnabled = false,
						depthStateEnabled = false,
					},
				},
			},
		},
		transformation = transformation,
	}
end

