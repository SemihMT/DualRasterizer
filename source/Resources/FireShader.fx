// Matrices
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WorldMatrix;
float4x4 gViewInverseMatrix : ViewInverseMatrix;
// Necessary Textures
Texture2D gDiffuseMap : DiffuseMap;

RasterizerState gRasterizerStateNoCull
{
    CullMode = none;
    FrontCounterClockwise = false;
};
RasterizerState gRasterizerStateFrontCull
{
    CullMode = front;
    FrontCounterClockwise = false;
};
RasterizerState gRasterizerStateBackCull
{
    CullMode = back;
    FrontCounterClockwise = false;
};

BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;

    //the rest is redundant because StencilEnable == false
    //demo:
    // StencilReadMask = 0x0F;
    // StencilWriteMask = 0x0F;

    // FrontFaceStencilFunc = always;
    // BackFaceStencilFunc = always;

    // FrontFaceStencilDepthFail = keep;
    // BackFaceStencilDepthFail = keep;

    // FrontFaceStencilPass = keep;
    // BackFaceStencilPass = keep;

    // FrontFaceStencilFail = keep;
    // BackFaceStencilFail = keep;
};

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap; //or Mirror, Clamp, Border
    AddressV = Wrap; //or Mirror, Clamp, Border
};
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap; //or Mirror, Clamp, Border
    AddressV = Wrap; //or Mirror, Clamp, Border
};
SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap; //or Mirror, Clamp, Border
    AddressV = Wrap; //or Mirror, Clamp, Border
};
//Input/Output structs
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

//Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position,1.f), gWorldViewProj);
    output.UV = input.UV;
    return output;
}


// Pixel Shader Point
float4 PSPoint(VS_OUTPUT input) : SV_TARGET
{
   
    return gDiffuseMap.Sample(samPoint,input.UV);
    //return float4(input.Color,1.f);
 


   
}

// Pixel Shader Linear
float4 PSLinear(VS_OUTPUT input) : SV_TARGET
{
       return gDiffuseMap.Sample(samLinear,input.UV);

}

// Pixel Shader Anisotropic
float4 PSAnisotropic(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(samAnisotropic,input.UV);

}

// Techniques
technique11 PointFilteringNoCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateNoCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSPoint() ) );
    }
}

technique11 LinearFilteringNoCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateNoCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSLinear() ) );
    }
}

technique11 AnisotropicFilteringNoCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateNoCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSAnisotropic() ) );
    }
}


technique11 PointFilteringFrontCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateFrontCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSPoint() ) );
    }
}

technique11 LinearFilteringFrontCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateFrontCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSLinear() ) );
    }
}

technique11 AnisotropicFilteringFrontCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateFrontCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSAnisotropic() ) );
    }
}

technique11 PointFilteringBackCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateBackCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSPoint() ) );
    }
}

technique11 LinearFilteringBackCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateBackCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSLinear() ) );
    }
}

technique11 AnisotropicFilteringBackCull
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateBackCull);
        SetDepthStencilState(gDepthStencilState,0);
        SetBlendState(gBlendState,float4(0.0f,0.0f,0.0f,0.0f),0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(ps_5_0, PSAnisotropic() ) );
    }
}