cbuffer SceneConstantBuffer : register(b0)
{
	float4 offset;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

PSInput VSMain(float4 Position : POSITION, float4 Color : COLOR)
{
	PSInput Result;

	Result.Position = Position + offset;
	Result.Color = Color;

	return Result;
}