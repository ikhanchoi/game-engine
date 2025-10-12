#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aTangent;
layout(location = 3) in vec2 aTexCoord_0;
// color, joints, weights may be added here

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vWorldPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out mat3 vTBN;

void main()
{
    vec4 worldPosition = uModel * vec4(aPosition, 1.0);
    vWorldPosition = worldPosition.xyz;

    // 일반(법선)을 월드 공간으로 변환
    vNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);

    // 텍스처 좌표 전달
    vTexCoord = aTexCoord_0;

    // TBN 행렬 구성 (노멀 매핑용)
    vec3 T = normalize(mat3(uModel) * aTangent.xyz);
    vec3 N = vNormal;
    vec3 B = cross(N, T) * aTangent.w;
    vTBN = mat3(T, B, N);

    gl_Position = uProjection * uView * worldPosition;
}
