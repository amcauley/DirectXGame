
/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

struct VIn
{
  float4 position : POSITION;
  float2 tex : TEXCOORD0;
};

struct PIn
{
  float4 position : SV_POSITION;
  float2 tex: TEXCOORD0;
};

cbuffer vs_const_buffer : register(b0)
{
  float4x4 totMat;
};

PIn VShader(VIn input)
{
    PIn output;
    output.position = mul(totMat, input.position);
    output.tex = input.tex;

    return output;
}


float4 PShader(PIn input) : SV_TARGET
{
    float4 textureColor;
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    return textureColor;
}