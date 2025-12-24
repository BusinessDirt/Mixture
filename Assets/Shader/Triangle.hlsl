[shader("vertex")]
float4 VSMain(float4 pos : POSITION) : SV_POSITION {
    return pos;
}

[shader("pixel")]
float4 PSMain() : SV_TARGET {
    return float4(1, 0, 0, 1);
}
