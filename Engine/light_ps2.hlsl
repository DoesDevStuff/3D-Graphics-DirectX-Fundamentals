// Light pixel shader
// Calculate diffuse lighting for a single directional light(also texturing)

Texture2D shaderTexture1 : register(t0);
Texture2D shaderTexture2 : register(t1);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightPosition;
	float padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float4	textureColor1;
	float4	textureColor2;
	float4	textureColorFinal;
	float3	lightDir;
	float	lightIntensity;
	float4	color;

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
	color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	//textureColor = shaderTexture.Sample(SampleType, input.tex);
	textureColor1 = shaderTexture1.Sample(SampleType, input.tex);
	textureColor2 = shaderTexture2.Sample(SampleType, input.tex);
	textureColorFinal = ((textureColor1 + textureColor2) / 2);

	color = color * textureColorFinal;

	/*to invert color, first calculate it and then subtract color from 1 since it's range is from 0 - 1*/
	//color = 1 - (color * textureColor);


	//method 2 subtract range first and then inverse it 
	//color = (color * textureColor - 1) * (-1);//

	return color;
}

