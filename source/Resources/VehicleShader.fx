// Matrices
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WorldMatrix;
float4x4 gViewInverseMatrix : ViewInverseMatrix;

// Necessary Textures
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;


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
    BlendEnable[0] = false;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = 1;
    DepthFunc = 4;
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

// Extra Variables
float3 gLightDirection = {0.577f,-0.577f,0.577f};
float1 gPI = 3.14159265359f;
float3 gAmbient = {0.025f,0.025f,0.025f};
float1 gLightIntensity = 7.0f;
float1 gShininess = 25.0f;

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
    float4 WorldPosition : W_POSITION;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

//Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position,1.f), gWorldViewProj);
    output.WorldPosition = mul(float4(input.Position,1.f),gWorldMatrix);
    output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
    output.Color = input.Color;
    output.UV = input.UV;
    return output;
}

float4 Lambert(float kd, float4 cd)
{
	return cd * kd / gPI;
}
float Phong(float ks, float exp,  float3 l,  float3 v,  float3 n)
{
	float3 reflected = reflect(l,n);
	float cosAlpha = saturate(dot(reflected, v));
	float specular = ks * pow(cosAlpha,exp);
	

	return specular;
}

// Pixel Shader Point
float4 PSPoint(VS_OUTPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);

    float3 binormal = cross(normalize(input.Normal),normalize(input.Tangent));
    float4x4 tangentSpaceAxis = float4x4(float4(input.Tangent, 0.0f), 
                                        float4(binormal, 0.0f), 
                                        float4(input.Normal, 0.0), 
                                        float4(0.0f, 0.0f, 0.0f, 1.0f));

	float3 normal = (2.f * gNormalMap.Sample(samPoint,input.UV).rgb) - float3(1.f,1.f,1.f);
	float3 sampledNormal = mul(float4(normal,0),tangentSpaceAxis);

	float4 lambert = Lambert(1.f, gDiffuseMap.Sample(samPoint,input.UV));
	float phongExp = gShininess * gGlossinessMap.Sample(samPoint,input.UV).r;
	float4 specular = gSpecularMap.Sample(samPoint,input.UV) *  Phong(1.0f,phongExp,-normalize(gLightDirection),viewDirection,normalize(sampledNormal));
	float observedArea =  saturate(dot(normalize(sampledNormal),-normalize(gLightDirection)));
	
	
    return ((gLightIntensity * lambert  + specular)* observedArea) + float4(gAmbient,.025f);
    //return float4(input.Color,1.f);
 


   
}

// Pixel Shader Linear
float4 PSLinear(VS_OUTPUT input) : SV_TARGET
{
     float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);

    float3 binormal = cross(normalize(input.Normal),normalize(input.Tangent));
    float4x4 tangentSpaceAxis = float4x4(float4(input.Tangent, 0.0f), 
                                        float4(binormal, 0.0f), 
                                        float4(input.Normal, 0.0), 
                                        float4(0.0f, 0.0f, 0.0f, 1.0f));

	float3 normal = (2.f * gNormalMap.Sample(samLinear,input.UV).rgb) - float3(1.f,1.f,1.f);
	float3 sampledNormal = mul(float4(normal,0),tangentSpaceAxis);

	float4 lambert = Lambert(1.f, gDiffuseMap.Sample(samLinear,input.UV));
	float phongExp = gShininess * gGlossinessMap.Sample(samLinear,input.UV).r;
	float4 specular = gSpecularMap.Sample(samLinear,input.UV) *  Phong(1.0f,phongExp,-normalize(gLightDirection),viewDirection,normalize(sampledNormal));
	float observedArea =  saturate(dot(normalize(sampledNormal),-normalize(gLightDirection)));
	
	
    return ((gLightIntensity * lambert  + specular)* observedArea) + float4(gAmbient,.025f);
}

// Pixel Shader Anisotropic
float4 PSAnisotropic(VS_OUTPUT input) : SV_TARGET
{
      float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);

    float3 binormal = cross(normalize(input.Normal),normalize(input.Tangent));
    float4x4 tangentSpaceAxis = float4x4(float4(input.Tangent, 0.0f), 
                                        float4(binormal, 0.0f), 
                                        float4(input.Normal, 0.0), 
                                        float4(0.0f, 0.0f, 0.0f, 1.0f));

	float3 normal = (2.f * gNormalMap.Sample(samAnisotropic,input.UV).rgb) - float3(1.f,1.f,1.f);
	float3 sampledNormal = mul(float4(normal,0),tangentSpaceAxis);

	float4 lambert = Lambert(1.f, gDiffuseMap.Sample(samAnisotropic,input.UV));
	float phongExp = gShininess * gGlossinessMap.Sample(samAnisotropic,input.UV).r;
	float4 specular = gSpecularMap.Sample(samAnisotropic,input.UV) *  Phong(1.0f,phongExp,-normalize(gLightDirection),viewDirection,normalize(sampledNormal));
	float observedArea =  saturate(dot(normalize(sampledNormal),-normalize(gLightDirection)));
	
	
    return ((gLightIntensity * lambert  + specular)* observedArea) + float4(gAmbient,.025f);
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