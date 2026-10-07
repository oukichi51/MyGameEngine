cbuffer ObjectConstants : register(b0)
{
    float4x4 WorldViewProjection;
    float4x4 World;
    float4 MaterialColor;
    float3 LightDirection;
    float Padding;
};

Texture2D DiffuseTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct VSInput { float3 position : POSITION; float3 normal : NORMAL; float2 uv : TEXCOORD; };
struct PSInput { float4 position : SV_POSITION; float3 normal : NORMAL; float2 uv : TEXCOORD; };

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position, 1), WorldViewProjection);
    output.normal = normalize(mul(float4(input.normal, 0), World).xyz);
    output.uv = input.uv;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float diffuse = saturate(dot(normalize(input.normal), -normalize(LightDirection)));
    return DiffuseTexture.Sample(LinearSampler, input.uv) * MaterialColor * (0.2 + diffuse * 0.8);
}
