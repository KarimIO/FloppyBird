@echo off

call "shaders/dxc.exe" -E vertexMain -T vs_6_0 -Zi /Fo shaders/vertex.cso /Fd shaders/vertex.pdb shaders/shader.hlsl
call "shaders/dxc.exe" -E fragmentMain -T ps_6_0 -Zi /Fo shaders/fragment.cso /Fd shaders/fragment.pdb shaders/shader.hlsl
echo Built shaders.
