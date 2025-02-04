#version 460 core

layout(location = 0)out vec4 colour;
layout(location = 1)out vec4 uv;

in vec2 texCoords;

uniform vec2 MousePos;
uniform float Progress;

uniform vec2 u_ScreenSize;
uniform sampler2D u_GroundTexture;

float pi = 3.1415;
float sizeOfRing = 15;

void main()
{
    float ScreenPixelSize = 1.0/min(u_ScreenSize.x, u_ScreenSize.y);
    colour = texture(u_GroundTexture, texCoords);
    uv = vec4(texCoords, 0, 1);
    vec2 localToMouse = (texCoords - MousePos);
    float RfromM = distance(localToMouse, vec2(0));
    if (RfromM < ScreenPixelSize * sizeOfRing && RfromM > ScreenPixelSize * (sizeOfRing - 5) && atan(-localToMouse.x, -localToMouse.y) < (Progress * pi * 2) - pi) {
        colour = vec4(vec3(1), 1);
    }
    if (RfromM < ScreenPixelSize * (sizeOfRing - 5)) {
        colour = mix(colour, vec4(vec3(1), 1), 0.5f);
    }

}

//float blume() {
//    float Lum = 0;
//
//    mat3 Gaussian;
//    Gaussian[0][0] = 1;
//    Gaussian[0][1] = 2;
//    Gaussian[0][2] = 1;
//    Gaussian[1][0] = 2;
//    Gaussian[1][1] = 4;
//    Gaussian[1][2] = 2;
//    Gaussian[2][0] = 1;
//    Gaussian[2][1] = 2;
//    Gaussian[2][2] = 1;
//
//    for (int i = 0; i < 3; i++) {
//        for (int j = 0; j < 3; j++) {
//            Lum = relativeLuminance(Samples[(i * 3) + j]) * Gaussian[i][j];
//        }
//    }
//    return Lum/16;
//}
//
//vec3 sobel() {
//    // get samples around pixel
//
//
//    mat3 GaussianDiff;
//    GaussianDiff[0][0] = intensity(Samples[0]);
//    GaussianDiff[0][1] = intensity(Samples[1]);
//    GaussianDiff[0][2] = intensity(Samples[2]);
//    GaussianDiff[1][0] = intensity(Samples[3]);
//    GaussianDiff[1][1] = intensity(Samples[4]);
//    GaussianDiff[1][2] = intensity(Samples[5]);
//    GaussianDiff[2][0] = intensity(Samples[6]);
//    GaussianDiff[2][1] = intensity(Samples[7]);
//    GaussianDiff[2][2] = intensity(Samples[8]);
//
//    mat3 xSobel;
//    xSobel[0][0] = 1;
//    xSobel[0][1] = 2;
//    xSobel[0][2] = 1;
//    xSobel[1][0] = 0;
//    xSobel[1][1] = 0;
//    xSobel[1][2] = 0;
//    xSobel[2][0] = -1;
//    xSobel[2][1] = -2;
//    xSobel[2][2] = -1;
//
//    mat3 ySobel;
//    ySobel[0][0] = -1;
//    ySobel[0][1] = 0;
//    ySobel[0][2] = 1;
//    ySobel[1][0] = -2;
//    ySobel[1][1] = 0;
//    ySobel[1][2] = 2;
//    ySobel[2][0] = -1;
//    ySobel[2][1] = 0;
//    ySobel[2][2] = 1;
//
//
//    // Sobel masks (see http://en.wikipedia.org/wiki/Sobel_operator)
//    //        1 0 -1     -1 -2 -1
//    //    X = 2 0 -2  Y = 0  0  0
//    //        1 0 -1      1  2  1
//
//    // You could also use Scharr operator:
//    //        3 0 -3        3 10   3
//    //    X = 10 0 -10  Y = 0  0   0
//    //        3 0 -3        -3 -10 -3
//
//    
//
//    float x = 0;// tleft + 2.0 * left + bleft - tright - 2.0 * right - bright;
//    float y = 0;// -tleft - 2.0 * top - tright + bleft + 2.0 * bottom + bright;
//
//    for (int i = 0; i < 3; i++) {
//        for (int j = 0; j < 3; j++) {
//            x += xSobel[i][j] * GaussianDiff[i][j];
//            y += ySobel[i][j] * GaussianDiff[i][j];
//        }
//    }
//
//    float r = (((atan(x, y) * 2.0) + 3.1415)) / (4.0 * 3.1415);
//    float n = sqrt((x * x) + (y * y));
//    if (n < 0.001){
//        return vec3(0, 0, 0);
//    }
//
//    return vec3(-x / n, y / n, n);
//
//}
//
//float LinearizeDepth(sampler2D diffuseMap ,vec2 uv, float factor)
//{
//    float n = 1.0; // camera z near
//    float f = zPlane * factor; // camera z far
//    float z =  texture2D(diffuseMap, uv).x;
//    if (1 < 1) {
//        return 1 - ((2.0 * n) / (f + n - z * (f - n)));
//    }
//    return ((2.0 * n) / (f + n - z * (f - n)));
//}
//
//float intensity(in vec4 color) {
//    color = ((floor(color * u_Factor) + 0.5) / (Dist2 * u_Factor));
//     return sqrt((color.x * color.x) + (color.y * color.y) + (color.z * color.z));
// 
//}
//
//float relativeLuminance(vec4 colour) {
//    return (colour.r * 0.2126) + (colour.g * 0.7152) + (colour.b * 0.0722);
//}
//
//vec3 rgb2hsv(vec3 c)
//{
//    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
//    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
//    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
//
//    float d = q.x - min(q.w, q.y);
//    float e = 1.0e-10;
//    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
//}
//
//vec3 hsv2rgb(vec3 c)
//{
//    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
//    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
//    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
//}
//
//vec2 rotate2D(vec2 a, float t) {
//    float c = cos(t);
//    float s = sin(t);
//    return vec2((c * a.x) - (s * a.y), (s * a.x) + (c * a.y));
//}
