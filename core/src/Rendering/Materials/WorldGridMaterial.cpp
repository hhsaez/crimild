/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/Materials/WorldGridMaterial.hpp"

using namespace crimild;
using namespace crimild::materials;

WorldGrid::WorldGrid( void ) noexcept
{
    setProgram(
        std::make_shared< ShaderProgram >(
            Array< SharedPointer< Shader > > {
                std::make_shared< Shader >(
                    Shader::Stage::FRAGMENT,
                    R"(
                        // Based on https://www.techarthub.com/unity-grid-shader/

                        vec3 gridLines( float size, float lineWidth, vec3 P, vec3 N )
                        {
                            vec3 G = mod( P, size / 100.0 );
                            G = G * G;
                            vec3 s = vec3(
                                step( G.x, lineWidth / 100.0 ),
                                step( G.y, lineWidth / 100.0 ),
                                step( G.z, lineWidth / 100.0 )
                            );

                            vec3 grid = vec3(
                                dot(
                                    vec3(
                                        mix(
                                            mix(
                                                s.r + s.b,
                                                s.g + s.b,
                                                abs( N.x )
                                            ),
                                            s.r + s.g,
                                            abs( N.z )
                                        )
                                    ),
                                    vec3( 1.0 )
                                )
                            );

                            return grid;
                        }

                        vec3 checker( float size, vec3 P, vec3 N )
                        {
                            float PTC = 32.0;
                            float contrast = 0.5;
                            vec3 wallColor = vec3( 0.5, 0.5, 0.6 );
                            vec3 floorColor = vec3( 0.5, 0.5, 0.6 );

                            vec3 A0 = P * vec3( -1.0 );
                            float A1 = size / 100.0;

                            vec3 B0 = mod( A0, vec3( A1 ) );
                            vec3 B1 = mod( P, vec3( A1 ) );
                            vec3 B2 = vec3( A1 / 2.0 );
                            vec3 B3 = P * vec3( 100.0, 100.0, 100.0 );

                            vec3 C0 = B0 - B2;
                            vec3 C1 = B1 - B2;

                            vec3 D0 = C0 * vec3( -1 );
                            vec3 D1 = C1;
                            vec3 D2 = clamp( B3, vec3( 0 ), vec3( 1.0 ) );

                            vec3 E0 = mix( D0, D1, D2 );

                            float F0 = E0.x;
                            float F1 = E0.y;
                            float F2 = E0.z;

                            float F3 = 0 - PTC;
                            float F4 = PTC + 1;
                            float F5 = abs( N.z );
                            float F6 = abs( N.y );

                            float G0 = F2 * F1;
                            float G1 = F0 * F1;
                            float G2 = F2 * F0;

                            float G3 = mix( F3, F4, F5 );
                            float G4 = mix( F3, F4, F6 );

                            float H0 = step( G0, 0 );
                            float H1 = step( G1, 0 );
                            float H2 = step( G2, 0 );

                            float H3 = clamp( G3, 0, 1 );
                            float H4 = clamp( G4, 0, 1 );

                            float I0 = mix( H0, H1, H3 );

                            float J0 = mix( I0, H2, H4 );

                            float K0 = mix( 1.0, J0, contrast );
                            vec3 K1 = mix( wallColor, floorColor, H4 );

                            return K0 * K1;
                        }

                        void frag_main( inout Fragment frag )
                        {
                            vec3 P = frag.scale * frag.modelPosition;
                            vec3 N = normalize( frag.modelNormal );
                            float Z = frag.depth;

                            float size = 100.0;
                            vec3 lineColor = vec3( 1 );
                            float lineWidth = 0.1;

                            float secondaryGridSize = 0.5 * size;
                            float secondaryLineWidth = 0.5 * lineWidth;
                            float secondaryGridIntensity = 0.0;
                            float secondaryGridFadeOffset = 0.0f;
                            float secondaryGridFadeLength = 1.0;

                            float secondaryCheckerIntensity = 0.0;

                            vec3 primaryGrid = lineColor * gridLines( size, lineWidth, P, N );
                            vec3 primaryChecker = checker( size, P, N );
                            vec3 primaryColor = primaryGrid + primaryChecker;

                            float secondaryGridFalloff = ( Z - secondaryGridFadeOffset ) / secondaryGridFadeLength;
                            secondaryGridFalloff = 1.0 - clamp( secondaryGridFalloff, 0.0, 1.0 );
                            vec3 secondaryGrid = secondaryGridIntensity * lineColor * secondaryGridFalloff * gridLines( secondaryGridSize, secondaryLineWidth, P, N );

                            vec3 secondaryChecker = secondaryCheckerIntensity * checker( 0.5 * size, P, N );

                            vec3 color = primaryColor + secondaryGrid;
                            color = clamp( color, 0.0, 1.0 );
                            color = color + secondaryChecker;

                            frag.albedo = color;
                        }
                    )" ),
            } ) );
}
