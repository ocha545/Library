cbuffer b1 : register(b1)
{
	float4 Diffuse;
}

cbuffer b2 : register(b2)
{
	float2 Position;
	float Radius;
	float Edge;
}

cbuffer b3 : register(b3)
{
	int Type;
}

struct PS_IN
{
//	float4 position : POSITION0;
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};


Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

#define TYPE_SHAPE 1
#define TYPE_TEXTURE 2

float4 psMain(PS_IN i) : SV_TARGET
{
	if(Type == TYPE_SHAPE)
	{
		return Diffuse;
	}
	else if(Type == TYPE_TEXTURE)
	{
		return Texture.Sample(Sampler, i.uv) * Diffuse;
//		return float4(i.uv.x, i.uv.y, 0, 1);
	}
	else
	{
		return float4(i.uv.x, i.uv.y, 1, (i.uv.x + i.uv.y) / 2);
	}
}

