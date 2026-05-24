#version 410

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;

layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;
	
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
	
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
	
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;
	
void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(pos, 1.0f);
            totalNormal = norm;
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos, 1.0f);
        totalPosition += localPosition * weights[i];

        totalNormal += mat3(finalBonesMatrices[boneIds[i]]) * norm * weights[i];
    }

    // If no bones influenced this vertex, use original position
    if(totalPosition == vec4(0.0f)) {
        totalPosition = vec4(pos, 1.0f);
        totalNormal = norm;
    }

    mat4 viewModel = view * model;
    gl_Position = projection * viewModel * totalPosition;

    normal = normalize(totalNormal);
    fragPos = vec3(model * totalPosition);
    texCoord = tex;
}