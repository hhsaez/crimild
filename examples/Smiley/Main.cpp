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

#include <Crimild.hpp>

using namespace crimild;

class Example : public Simulation {
public:
    void onStarted( void ) noexcept override
    {
        RenderSystem::getInstance()->setFrameGraph(
            [] {
                using namespace crimild::framegraph;
                return shader(
                    R"(
                        float circleMask( vec2 uv, vec2 p, float r, float blur )
                        {
                            float d = length( uv - p );
                            float c = smoothstep( r, r - blur, d );
                            return c;
                        }

                        void main()
                        {
                            vec2 uv = inTexCoord;
                            uv -= 0.5;
                            uv.x *= context.dimensions.x / context.dimensions.y;

                            float blur = 0.00625;

                            float mask = circleMask( uv, vec2( 0.0 ), 0.4, blur );
                            mask -= circleMask( uv, vec2( -0.15, 0.1 ), 0.075, blur );
                            mask -= circleMask( uv, vec2( 0.15, 0.1 ), 0.075, blur );
                            vec3 faceColor = vec3( 1.0, 1.0, 0.0 ) * mask;

                            mask = circleMask( uv, vec2( 0.0 ), 0.25, blur );
                            mask -= circleMask( uv, vec2( 0.0, 0.05 ), 0.25, blur );
                            mask *= uv.y <= 0.0 ? 1.0 : 0.0;
                            vec3 mouthColor = vec3( 1.0 ) * mask;

                            vec3 color = faceColor - mouthColor;

                            outColor = vec4( color, 1.0 );
                        }
                    )" );
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Smiley" );
