struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

float4 PSMain(PSInput Input) : SV_TARGET
{
	return Input.Color;
}
