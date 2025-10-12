#version 330 core
in vec3 vWorldPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in mat3 vTBN;

uniform sampler2D tBaseColorTexture;
uniform sampler2D tMetallicRoughnessTexture;
uniform sampler2D tNormalTexture;
uniform sampler2D tOcclusionTexture;
uniform sampler2D tEmissiveTexture;
uniform vec3  tBaseColorFactor;     // usually [1,1,1]
uniform float tMetallicFactor;      // usually 0.0 ~ 1.0
uniform float tRoughnessFactor;     // usually 0.0 ~ 1.0
uniform vec3  tEmissiveFactor;      // usually [0,0,0]

uniform vec3 uCameraPosition;
uniform vec3 uLightPosition; // single light
uniform vec3 uLightColor;

out vec4 FragColor; // location = 0 as default

// ----------------------------------------------------------------------------

// GGX Normal Distribution Function (NDF)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265 * denom * denom;
    return a2 / denom;
}

// Schlick-GGX Geometry function (for a single direction)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

// Smith's method for combined geometry attenuation
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// Fresnel term using Schlick's approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ----------------------------------------------------------------------------

void main()
{
    // texture sampling
    vec3  baseColor = texture(tBaseColorTexture, vTexCoord).rgb * tBaseColorFactor;
    float metalness = texture(tMetallicRoughnessTexture, vTexCoord).b * tMetallicFactor;
    float roughness = texture(tMetallicRoughnessTexture, vTexCoord).g * tRoughnessFactor;
    float occlusion = texture(tOcclusionTexture, vTexCoord).r;

//*
    // normal map processing (instead of computed normal vector)
    vec3 N = vNormal;
    vec3 tangentNormal = texture(tNormalTexture, vTexCoord).rgb;
    tangentNormal = tangentNormal * 2.0 - 1.0; // [0,1] -> [-1,1]
    N = normalize(vTBN * tangentNormal);

    vec3 V = normalize(uCameraPosition - vWorldPosition); // view vector
    vec3 L = normalize(uLightPosition - vWorldPosition); // light direction
    vec3 H = normalize(V + L);

    vec3 F0 = mix(vec3(0.04), baseColor, metalness); // baseColor for metal, 0.04 for non-metal

    // elements in PBR
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    // Cook-Torrance BRDF
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = (NDF * G * F) / denominator;

    // kS: specular reflection coeff, kD: diffuse coeff
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;

    // reflection by light? (light coloe, NdotL)
    float NdotL = max(dot(N, L), 0.0);
    vec3 radiance = uLightColor; // 조명 강도 고려 가능
    vec3 diffuse = (baseColor / 3.14159265);
    vec3 Lo = (kD * diffuse + specular) * radiance * NdotL;

/*/

    vec3 N = normalize(vTBN * (texture(normalTexture, vTexCoord).rgb * 2.0 - 1.0));
    vec3 V = normalize(uCameraPos - vWorldPos);
    vec3 L = normalize(uLightPos - vWorldPos);
    vec3 H = normalize(V + L);

    vec3 F0 = mix(vec3(0.04), baseColor, metalness);

    float NDF = max(dot(N, H), 0.0);
    float G = max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - max(dot(H, V), 0.0), 5.0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metalness;

    float NdotL = max(dot(N, L), 0.0);
    vec3 radiance = uLightColor;

    vec3 diffuse = baseColor / 3.14159265;
    vec3 specular = (NDF * G * F) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001);
    vec3 Lo = (kD * diffuse + specular) * radiance * NdotL;

//*/


    vec3 ambient = vec3(0.03) * baseColor * occlusion; //(would be better if used HDR map)
    vec3 emissive = texture(tEmissiveTexture, vTexCoord).rgb * tEmissiveFactor;
    vec3 color = ambient + Lo + emissive;

    // gamma correction (gamma 2.2)
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);


}
