struct PS_INPUT {
    float4 hPosition : POSITION;
    float2 depth : TEXCOORD;
};

struct PS_OUTPUT {
    float4 color : COLOR;
    float depth : DEPTH;
};

PS_OUTPUT main(PS_INPUT input) {
    PS_OUTPUT output = (PS_OUTPUT)0;
    output.depth = input.depth.x;
    return output;
}