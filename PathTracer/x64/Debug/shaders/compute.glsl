#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D shaderTexture;
layout(rgba32f, binding = 1) uniform image2D accumulationTexture;

uniform int width;
uniform int height;
uniform float totalFrames;

void main(){
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    if (pixelCoords.x >= width || pixelCoords.y >= height) return;

    vec4 color = texelFetch(shaderTexture, pixelCoords, 0);
    vec4 accumColor = imageLoad(accumulationTexture, pixelCoords);

    accumColor = (accumColor * totalFrames + color) / (totalFrames + 1.0f);

    imageStore(accumulationTexture, pixelCoords, accumColor);
}
