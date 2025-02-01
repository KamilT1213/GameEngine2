#version 460 core

out vec4 colour;

in vec2 texCoords;

uniform sampler2D u_inputTexture;
uniform sampler2D u_inputDepthTexture;
uniform sampler2D u_gCubeMap;
uniform sampler2D u_UVmap;
uniform sampler2D u_terrainDisplacmentMap;
uniform vec2 u_posOnMap;
uniform float u_BrushSize;
uniform vec3 u_SurfaceNormal;
uniform float u_ToggleShader;

uniform float u_Factor;
uniform float u_Toggle;

struct directionalLight
{
    vec3 colour;
    vec3 direction;
};

struct pointLight
{
    vec3 colour;
    vec3 position;
    vec3 constants;
};

struct spotLight
{
    vec3 colour;
    vec3 position;
    vec3 direction;
    vec3 constants;
    float cutOff;
    float outerCutOff;
};

const int numPointLights = 10;
const int numSpotLights = 1;

layout(std140, binding = 1) uniform b_lights
{
    uniform directionalLight dLight;
    uniform pointLight pLights[numPointLights];
    uniform spotLight sLights[numSpotLights];
};

layout(std140, binding = 0) uniform b_camera
{
    uniform mat4 u_view;
    uniform mat4 u_projection;
    uniform vec3 u_viewPos;
};

uniform vec3 u_HSVColour;
uniform vec2 u_ScreenSize;
uniform mat4 u_ScreenMatrix;

float intensity(in vec4 color);

float relativeLuminance(vec4 colour);
float LinearizeDepth(sampler2D diffuseMap, vec2 uv, float factor);
float blume();
vec2 rotate2D(vec2 a, float t);

vec3 sobel();

vec3 rgb2hsv(vec3 c);

vec3 hsv2rgb(vec3 c);

float zPlane = 1000;

float Dist2;

vec4 Samples[9];


void main()
{

    vec2 texelSize = vec2(1 / u_ScreenSize.x / 2.0, 1 / u_ScreenSize.y / 2.0);
    Samples[0] = pow(texture(u_inputTexture, texCoords + vec2(-texelSize.x, texelSize.y)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));
    Samples[1] = pow(texture(u_inputTexture, texCoords + vec2(-texelSize.x, 0)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));
    Samples[2] = pow(texture(u_inputTexture, texCoords + vec2(-texelSize.x, -texelSize.y)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));
    Samples[3] = pow(texture(u_inputTexture, texCoords + vec2(0, texelSize.y)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));
    Samples[4] = pow(texture(u_inputTexture, texCoords + vec2(0, 0)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));
    Samples[5] = pow(texture(u_inputTexture, texCoords + vec2(0, -texelSize.y)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));
    Samples[6] = pow(texture(u_inputTexture, texCoords + vec2(texelSize.x, texelSize.y)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));
    Samples[7] = pow(texture(u_inputTexture, texCoords + vec2(texelSize.x, 0)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));
    Samples[8] = pow(texture(u_inputTexture, texCoords + vec2(texelSize.x, -texelSize.y)), vec4(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2, 1));


   
        //
    //fragmentPosLightSpace = u_lightSpaceTransform * vec4(fragmentPos, 1.0);

    //vec4(texCoords,0);//
    float Dist = LinearizeDepth(u_inputDepthTexture, texCoords, 0.2);
    if (Dist < 0.05) {
        Dist = 1;
    }

    Dist2 = (LinearizeDepth(u_inputDepthTexture, texCoords,1)).r;
    if (Dist2 > 0.95) {
        Dist2 = 0.2;
    }
    
    float L = length(sobel() * vec3(0, 0, (Dist/2)));// u_Dir);

    vec3 A = ((floor(texture(u_inputTexture, texCoords).xyz * u_Factor) + 0.5 )/ u_Factor);

    vec4 Acolour = vec4(A, 1.0);
    vec3 Bcolour = rgb2hsv(Acolour.rgb + hsv2rgb(u_HSVColour - vec3(0.5)));
    //Bcolour += ();// +(vec3(0, 0, blume()) * u_Toggle);
    Acolour = vec4(hsv2rgb(Bcolour) , 1.0);
    float fil = texture(u_gCubeMap, texCoords).r;
    if(fil < 0.3){
        fil = 0.0;
    }
    colour = mix((Acolour * (1 - L)) + (blume() * 2 ), vec4(0.2, 0.2, 0.2, 1.0), fil);// +vec4(texture(u_gCubeMap, texCoords).rbg / 2, 1.0);;




    vec2 onModelCoords = texture(u_UVmap, texCoords).xy;
    vec2 inCentreCoords = texture(u_UVmap, vec2(0.5)).xy;
    vec3 Displacememt = abs(texture(u_terrainDisplacmentMap, inCentreCoords).rgb - vec3(0.5));
    Displacememt *= pow(distance(vec3(0), Displacememt), 2);

    float disFactor = (1 / ((1 + (Displacememt.x + Displacememt.y + Displacememt.z)) ));

    vec3 normal = vec3(u_SurfaceNormal.x, abs(u_SurfaceNormal.y), u_SurfaceNormal.z) + vec3(0.001);
    vec3 rightTan = normalize(cross(normal, vec3(0, -1, 0)));
    vec3 upTan = normalize(cross(-rightTan, normal));
    float angle = acos(dot(vec3(0, 0, 1), rightTan));
        
    float scale = sqrt(disFactor) / 2.0;

    vec2 transformed = rotate2D(onModelCoords - inCentreCoords, (3.1415) - (angle * sign(dot(upTan, vec3(0, 0, 1)))));
    //transformed.x = (transformed.x - (sin(acos(dot(vec3(0, 1, 0), normal))) * disFactor)) * (1 / dot(vec3(0, 1, 0), normal));
    transformed += inCentreCoords;//transformed += inCentreCoords;
    

    colour = mix(texture(u_inputTexture, texCoords), colour, u_ToggleShader);

    //onModelCoords *= disFactor;
    if (distance(u_posOnMap / 4096.0, transformed)/ scale > ((u_BrushSize * disFactor)/ 4096.0) - 0.001 && distance(u_posOnMap / 4096.0, transformed)/ scale < ((u_BrushSize * disFactor) / 4096.0) + 0.001) {
        colour += vec4(vec3(0.3),1.0);
    }
    
    

    if (distance((texCoords - vec2(0.5)) * vec2(1, u_ScreenSize.y/ u_ScreenSize.x), vec2(0)) < 0.005 + (sin(atan(((texCoords - vec2(0.5)).x / (texCoords - vec2(0.5)).y)) * 100) * 0.0025)) {
        colour = mix(colour,vec4(vec3(0.6,0.1,0.1), 1.0),0.7);
    }

    mat4 VP = u_projection * u_view;

    for (int i = 0; i < numPointLights; i++)
    {
        //pLights[i].position
        vec2 lightPosOnScreen = (((((VP * vec4(pLights[i].position, 1.0)) / (VP * vec4(pLights[i].position, 1.0)).w).xy) + vec2(1)) * 0.5);//*u_ScreenSize;
        float dist = length(u_viewPos - pLights[i].position);
        if (distance((texCoords - lightPosOnScreen) * vec2(1, u_ScreenSize.y / u_ScreenSize.x) * (dist), vec2(0))   < 0.3 && (VP * vec4(pLights[i].position, 1.0)).w > 0 && u_Toggle > 0) {
            colour = mix(colour, vec4(pLights[i].colour, 1.0), 1.0);
        }
    }

    //color += LinearizeDepth(u_inputDepthTexture, texCoords, 0.2);
}

float blume() {
    float Lum = 0;

    mat3 Gaussian;
    Gaussian[0][0] = 1;
    Gaussian[0][1] = 2;
    Gaussian[0][2] = 1;
    Gaussian[1][0] = 2;
    Gaussian[1][1] = 4;
    Gaussian[1][2] = 2;
    Gaussian[2][0] = 1;
    Gaussian[2][1] = 2;
    Gaussian[2][2] = 1;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Lum = relativeLuminance(Samples[(i * 3) + j]) * Gaussian[i][j];
        }
    }
    return Lum/16;
}

vec3 sobel() {
    // get samples around pixel


    mat3 GaussianDiff;
    GaussianDiff[0][0] = intensity(Samples[0]);
    GaussianDiff[0][1] = intensity(Samples[1]);
    GaussianDiff[0][2] = intensity(Samples[2]);
    GaussianDiff[1][0] = intensity(Samples[3]);
    GaussianDiff[1][1] = intensity(Samples[4]);
    GaussianDiff[1][2] = intensity(Samples[5]);
    GaussianDiff[2][0] = intensity(Samples[6]);
    GaussianDiff[2][1] = intensity(Samples[7]);
    GaussianDiff[2][2] = intensity(Samples[8]);

    mat3 xSobel;
    xSobel[0][0] = 1;
    xSobel[0][1] = 2;
    xSobel[0][2] = 1;
    xSobel[1][0] = 0;
    xSobel[1][1] = 0;
    xSobel[1][2] = 0;
    xSobel[2][0] = -1;
    xSobel[2][1] = -2;
    xSobel[2][2] = -1;

    mat3 ySobel;
    ySobel[0][0] = -1;
    ySobel[0][1] = 0;
    ySobel[0][2] = 1;
    ySobel[1][0] = -2;
    ySobel[1][1] = 0;
    ySobel[1][2] = 2;
    ySobel[2][0] = -1;
    ySobel[2][1] = 0;
    ySobel[2][2] = 1;


    // Sobel masks (see http://en.wikipedia.org/wiki/Sobel_operator)
    //        1 0 -1     -1 -2 -1
    //    X = 2 0 -2  Y = 0  0  0
    //        1 0 -1      1  2  1

    // You could also use Scharr operator:
    //        3 0 -3        3 10   3
    //    X = 10 0 -10  Y = 0  0   0
    //        3 0 -3        -3 -10 -3

    

    float x = 0;// tleft + 2.0 * left + bleft - tright - 2.0 * right - bright;
    float y = 0;// -tleft - 2.0 * top - tright + bleft + 2.0 * bottom + bright;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            x += xSobel[i][j] * GaussianDiff[i][j];
            y += ySobel[i][j] * GaussianDiff[i][j];
        }
    }

    float r = (((atan(x, y) * 2.0) + 3.1415)) / (4.0 * 3.1415);
    float n = sqrt((x * x) + (y * y));
    if (n < 0.001){
        return vec3(0, 0, 0);
    }

    return vec3(-x / n, y / n, n);

}

float LinearizeDepth(sampler2D diffuseMap ,vec2 uv, float factor)
{
    float n = 1.0; // camera z near
    float f = zPlane * factor; // camera z far
    float z =  texture2D(diffuseMap, uv).x;
    if (1 < 1) {
        return 1 - ((2.0 * n) / (f + n - z * (f - n)));
    }
    return ((2.0 * n) / (f + n - z * (f - n)));
}

float intensity(in vec4 color) {
    color = ((floor(color * u_Factor) + 0.5) / (Dist2 * u_Factor));
     return sqrt((color.x * color.x) + (color.y * color.y) + (color.z * color.z));
 
}

float relativeLuminance(vec4 colour) {
    return (colour.r * 0.2126) + (colour.g * 0.7152) + (colour.b * 0.0722);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec2 rotate2D(vec2 a, float t) {
    float c = cos(t);
    float s = sin(t);
    return vec2((c * a.x) - (s * a.y), (s * a.x) + (c * a.y));
}
