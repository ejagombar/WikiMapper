#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool horizontal;
uniform int boarder;
uniform int radius;
uniform float blurScale;
uniform float brightnessModifier;

float weight[15] = float[](
        0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216,
        0.009216, 0.004216, 0.002016, 0.001016, 0.000716,
        0.000516, 0.000316, 0.000216, 0.000116, 0.000016
    );

bool isInsideRoundedRectangle(vec2 coord, vec2 screenSize, int boarder, int radius) {
    if ((coord.y > boarder) && (coord.x > boarder) && (coord.x < screenSize.x - boarder) && (coord.y < screenSize.y - boarder)) {
        vec2 topLeftCorner = vec2(boarder + radius, boarder + radius);
        vec2 topRightCorner = vec2(screenSize.x - boarder - radius, boarder + radius);
        vec2 bottomLeftCorner = vec2(boarder + radius, screenSize.y - boarder - radius);
        vec2 bottomRightCorner = vec2(screenSize.x - boarder - radius, screenSize.y - boarder - radius);

        if (distance(coord, topLeftCorner) < radius || distance(coord, topRightCorner) < radius || distance(coord, bottomLeftCorner) < radius || distance(coord, bottomRightCorner) < radius) {
            return true;
        } else if ((coord.x > boarder + radius) && (coord.x < screenSize.x - boarder - radius)) {
            return true;
        } else if ((coord.y > boarder + radius) && (coord.y < screenSize.y - boarder - radius)) {
            return true;
        }
    }
    return false;
}

void main() {
    vec2 screenSize = vec2(textureSize(screenTexture, 0));
    vec2 tex_offset = 1.0 / screenSize; // Get screenSize of a single texel

    vec3 result = texture(screenTexture, TexCoords).rgb;

    if (isInsideRoundedRectangle(gl_FragCoord.xy, screenSize, boarder, radius)) {
        result *= weight[0];
        if (horizontal) {
            for (int i = 1; i < 15; ++i) {
                result += texture(screenTexture, TexCoords + vec2(tex_offset.x * i * blurScale, 0.0)).rgb * weight[i];
                result += texture(screenTexture, TexCoords - vec2(tex_offset.x * i * blurScale, 0.0)).rgb * weight[i];
            }
        }
        else {
            for (int i = 1; i < 15; ++i) {
                result += texture(screenTexture, TexCoords + vec2(0.0, tex_offset.y * i * blurScale)).rgb * weight[i];
                result += texture(screenTexture, TexCoords - vec2(0.0, tex_offset.y * i * blurScale)).rgb * weight[i];
            }
        }
        result *= brightnessModifier;
    }

    FragColor = vec4(result, 1.0);
}
