#if UNITY_EDITOR
namespace TUK.Editor
{
    internal struct ShaderData
    {
        internal string VertexShader;
        internal string HullShader;
        internal string DomainShader;
        internal string GeometryShader;
        internal string PixelShader;

        internal static ShaderData GetDefault()
        {
            ShaderData data = new();

            data.VertexShader = "Shaders/Standard.vert.cso";
            data.HullShader = "";
            data.DomainShader = "";
            data.GeometryShader = "";
            data.PixelShader = "Shaders/Standard.frag.cso";

            return data;
        }
    }
}
#endif
