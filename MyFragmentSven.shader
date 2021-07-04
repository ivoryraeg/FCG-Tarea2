#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
//uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main(){
    // Output color = color of the texture at the specified UV
    vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
    vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;

    vec3 LightColor = vec3(1,0,0);
	float LightPower = 50.0f;

    // Distance to the light
	float distance = length( LightPosition_worldspace - Position_worldspace );
    
    // Normal of the computed fragment, in camera space
    vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	//float cosTheta = clamp( dot( n,l ), 0,1 );
	float cosTheta = clamp(dot( n,l ),0,1);

    //color = MaterialAmbientColor;
    color = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*10-9);
}