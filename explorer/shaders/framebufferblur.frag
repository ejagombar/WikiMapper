#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D originalTexture;
uniform bool horizontal;
uniform bool lastPass;
uniform float blurScale;
uniform float brightnessModifier;
uniform int width;
uniform int height;
uniform int radius;

float weight[15] = float[](
        0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216,
        0.009216, 0.004216, 0.002016, 0.001016, 0.000716,
        0.000516, 0.000316, 0.000216, 0.000116, 0.000016
    );

bool isInsideRoundedRectangle(vec2 coord, vec2 center, float width, float height, float radius) {
    float halfWidth = width / 2.0;
    float halfHeight = height / 2.0;

    vec2 topLeft = vec2(center.x - halfWidth, center.y - halfHeight);
    vec2 bottomRight = vec2(center.x + halfWidth, center.y + halfHeight);

    if (coord.x >= topLeft.x && coord.x <= bottomRight.x && coord.y >= topLeft.y && coord.y <= bottomRight.y) {
        vec2 cornerTopLeft = vec2(topLeft.x + radius, topLeft.y + radius);
        vec2 cornerTopRight = vec2(bottomRight.x - radius, topLeft.y + radius);
        vec2 cornerBottomLeft = vec2(topLeft.x + radius, bottomRight.y - radius);
        vec2 cornerBottomRight = vec2(bottomRight.x - radius, bottomRight.y - radius);

        if (distance(coord, cornerTopLeft) < radius || distance(coord, cornerTopRight) < radius ||
                distance(coord, cornerBottomLeft) < radius || distance(coord, cornerBottomRight) < radius) {
            return true;
        } else if ((coord.x >= cornerTopLeft.x && coord.x <= cornerTopRight.x) ||
                (coord.y >= cornerTopLeft.y && coord.y <= cornerBottomLeft.y)) {
            return true;
        }
    }
    return false;
}

void main() {
    vec2 screenSize = vec2(textureSize(screenTexture, 0));
    vec2 tex_offset = 1.0 / screenSize;

    vec3 blurResult = texture(screenTexture, TexCoords).rgb;
    blurResult *= weight[0];

    if (horizontal) {
        for (int i = 1; i < 15; ++i) {
            blurResult += texture(screenTexture, TexCoords + vec2(tex_offset.x * i * blurScale, 0.0)).rgb * weight[i];
            blurResult += texture(screenTexture, TexCoords - vec2(tex_offset.x * i * blurScale, 0.0)).rgb * weight[i];
        }
    }
    else {
        for (int i = 1; i < 15; ++i) {
            blurResult += texture(screenTexture, TexCoords + vec2(0.0, tex_offset.y * i * blurScale)).rgb * weight[i];
            blurResult += texture(screenTexture, TexCoords - vec2(0.0, tex_offset.y * i * blurScale)).rgb * weight[i];
        }
    }
    blurResult *= brightnessModifier;

    vec3 originalResult = texture(originalTexture, TexCoords).rgb;

    if (lastPass) {
        if (isInsideRoundedRectangle(gl_FragCoord.xy, screenSize / 2, width, height, radius)) {
            FragColor = vec4(blurResult, 1.0);
        } else {
            FragColor = vec4(originalResult, 1.0);
        }
    } else {
        FragColor = vec4(blurResult, 1.0);
    }
}
