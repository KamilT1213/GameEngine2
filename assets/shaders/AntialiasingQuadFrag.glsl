#version 460 core

out vec4 colour;

in vec2 texCoords;

uniform sampler2D u_inputTexture;
uniform float u_Factor;

uniform vec2 u_ScreenSize;

vec4 Samples[9];


void main()
{

    vec2 texelSize = vec2(1 / u_ScreenSize.x, 1 / u_ScreenSize.y);
    Samples[0] = texture(u_inputTexture, texCoords + vec2(-texelSize.x, texelSize.y));
    Samples[1] = texture(u_inputTexture, texCoords + vec2(-texelSize.x, 0));
    Samples[2] = texture(u_inputTexture, texCoords + vec2(-texelSize.x, -texelSize.y));
    Samples[3] = texture(u_inputTexture, texCoords + vec2(0, texelSize.y));
    Samples[4] = texture(u_inputTexture, texCoords + vec2(0, 0));
    Samples[5] = texture(u_inputTexture, texCoords + vec2(0, -texelSize.y));
    Samples[6] = texture(u_inputTexture, texCoords + vec2(texelSize.x, texelSize.y));
    Samples[7] = texture(u_inputTexture, texCoords + vec2(texelSize.x, 0));
    Samples[8] = texture(u_inputTexture, texCoords + vec2(texelSize.x, -texelSize.y));

    float[9] gaussian = {
    1/16.0, 1/8.0, 1/16.0,
    1/8.0,  1/4.0, 1/8.0,
    1/16.0, 1/8.0, 1/16.0
    };

    float[9] gaussian2 = {
    1 / 9.0, 1 / 9.0, 1 / 9.0,
    1 / 9.0, 1 / 9.0, 1 / 9.0,
    1 / 9.0, 1 / 9.0, 1 / 9.0
    };

    vec4 total;

    for (int i = 0; i < 9; i++) {
        total += Samples[i] * mix(gaussian[i], gaussian2[i], u_Factor);
    }

    colour = total;
}

