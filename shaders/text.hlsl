struct QuadRenderData {
	float x;
	float y;
	float width;
	float height;
};


struct QuadRenderDataBuffer
{
	QuadRenderData quadRenderData[14];
};

static float2 resolution = float2(640.0f, 480.0f);

ConstantBuffer<QuadRenderDataBuffer> QuadRenderDataCB : register(b0);

static float2 positions[4] = {
	float2( 0.5f,  0.5f),
	float2(-0.5f,  0.5f),
	float2( 0.5f, -0.5f),
	float2(-0.5f, -0.5f)
};

struct VertexInput
{
	uint Vertex : SV_VertexID;
	uint Instance : SV_InstanceID;
};

struct FragmentInput
{
	float4 Position : SV_Position;
	float2 TexCoords : TEXCOORD0;
};

FragmentInput vertexMain(VertexInput IN)
{
	FragmentInput OUT;

	QuadRenderData quad = QuadRenderDataCB.quadRenderData[IN.Instance];

	float2 offset = float2(quad.x, quad.y);
	float2 size = float2(quad.width, quad.height);
	OUT.Position = float4((positions[IN.Vertex] * size + offset) * 2.0f / resolution, 0.0f, 1.0f);

	return OUT;
}

float4 fragmentMain( FragmentInput IN ) : SV_Target
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
