#version 330

//This is a fragment shader. If you've opened this file first, please open and read lambert.vert.glsl before reading on.
//Unlike the vertex shader, the fragment shader actually does compute the shading of geometry.
//For every pixel in your program's output screen, the fragment shader is run for every bit of geometry that particular pixel overlaps.
//By implicitly interpolating the position data passed into the fragment shader by the vertex shader, the fragment shader
//can compute what color to apply to its pixel based on things like vertex position, light position, and vertex color.

uniform sampler2D u_Texture; // The texture to be read from by this shader
//uniform sampler2D u_Normal; // The texture to be read from by this shader
//These are the interpolated values out of the rasterizer, so you can't know
//their specific values without knowing the vertices that contributed to them
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec2 fs_UV;
in float fs_Pow;
in float fs_Ani;

in vec4 fs_CameraPos;
in vec4 fs_Pos;

uniform int u_Time;

out vec4 out_Col;//This is the final output color that you will see on your screen for the pixel that is currently being processed.

void main()
{
    // TODO Homework 4
    // Calculate specular highlights
    vec4 FCvector = fs_Pos;
    vec4 FLvector = fs_LightVec;
    vec4 H = normalize((FCvector + FLvector) / 2);
    //vec4 N = texture(u_Normal, fs_UV);
    //N = normalize(N*2.0-1.0);

    vec4 N = normalize(fs_Nor);
    float exp = fs_Pow;
    float specularIntensity = max(pow(dot(H, N), exp), 0);

    // diffuse lighting calculation
    float lambertLight = dot(N, normalize(FLvector));
    lambertLight = clamp(lambertLight, 0, 1);

    float ambientIntensity = 0.3f;
    // color of ambient light
    vec4 modelColor = texture(u_Texture, fs_UV + fs_Ani * vec2(((u_Time)%16)/256.f, 0.f));
    vec4 modelcolor = modelColor * (specularIntensity + lambertLight + ambientIntensity);
    // Output the final color
    out_Col = vec4(modelcolor.rgb, modelColor.a);

}
