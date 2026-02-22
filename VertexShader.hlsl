cbuffer VSConstantBuffer : register(b0)
{
	float4x4 mat;
}

struct VSOutput
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD;
};

//VSOutputはピクセルシェーダーに渡されるよ
VSOutput vsMain(float3 position : POSITION0, float2 uv : TEXCOORD)
{
	VSOutput output;
	output.position = mul(mat, float4(position, 1.0f));
	output.uv = uv;

	return output;
}
