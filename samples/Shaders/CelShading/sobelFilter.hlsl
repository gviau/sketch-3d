SamplerState samplerState : register(s0);
Texture2D textureToFilter : register(t0);

cbuffer ScreenParameters_t : register(b0)
{
    float4 ScreenParams;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

float RgbToGrayscale(float3 rgb)
{
    return (rgb.r + rgb.g + rgb.b) / 3.0;
}

PS_OUTPUT main(PS_INPUT input)
{
    float stepU = 1.0 / ScreenParams.x;
    float stepV = 1.0 / ScreenParams.y;

    PS_OUTPUT output;

    // For each gradient part, we apply a sobel kernel on the grayscale value of the pixels
    float3 topLeftPixel = textureToFilter.Sample(samplerState, input.uv + float2(-stepU, -stepV)).rgb;
    float3 topPixel = textureToFilter.Sample(samplerState, input.uv + float2(0.0, -stepV)).rgb;
    float3 topRightPixel = textureToFilter.Sample(samplerState, input.uv + float2(stepU, -stepV)).rgb;
    float3 leftPixel = textureToFilter.Sample(samplerState, input.uv + float2(-stepU, 0.0)).rgb;
    float3 rightPixel = textureToFilter.Sample(samplerState, input.uv + float2(stepU, 0.0)).rgb;
    float3 bottomLeftPixel = textureToFilter.Sample(samplerState, input.uv + float2(-stepU, stepV)).rgb;
    float3 bottomPixel = textureToFilter.Sample(samplerState, input.uv + float2(0.0, stepV)).rgb;
    float3 bottomRightPixel = textureToFilter.Sample(samplerState, input.uv + float2(stepU, stepV)).rgb;

    float topLeft = RgbToGrayscale(topLeftPixel);
    float top = RgbToGrayscale(topPixel);
    float topRight = RgbToGrayscale(topRightPixel);
    float left = RgbToGrayscale(leftPixel);
    float right = RgbToGrayscale(rightPixel);
    float bottomLeft = RgbToGrayscale(bottomLeftPixel);
    float bottom = RgbToGrayscale(bottomPixel);
    float bottomRight = RgbToGrayscale(bottomRightPixel);

    float gx = 0.0;
    gx += topLeft * -1.0;
    gx += left * -2.0;
    gx += bottomLeft * -1.0;
    gx += topRight;
    gx += right * 2.0;
    gx += bottomRight;

    float gy = 0.0;
    gy += topLeft * -1.0;
    gy += top * -2.0;
    gy += topRight * -1.0;
    gy += bottomLeft;
    gy += bottom * 2.0;
    gy += bottomRight;

    float gradient = abs(gx) + abs(gy);
    output.color = (gradient > 1.0) ? float4(0.0, 0.0, 0.0, 1.0) : float4(1.0, 1.0, 1.0, 1.0);

    return output;
}