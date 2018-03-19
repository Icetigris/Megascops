struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

PSInput VSMain(float4 Position : POSITION, float4 Color : COLOR)
{
	PSInput Result;

	Result.Position = Position;
	Result.Color = Color;

	return Result;
}