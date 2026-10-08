# 렌더링 구조

- `RenderSubsystem`: 디바이스, 창별 스왑 체인, 명령 제출, GPU 완료 대기와 리소스 생성.
- `Renderer`: 현재 `RenderTarget`에 명령 기록. 직접 제출하거나 Present하지 않습니다.
- `RenderTarget`: 컬러 텍스처와 선택적인 D32 깊이 버퍼. 창 타겟은 백 버퍼를 참조합니다.
- `SwapChain`: 창 핸들, 네이티브 스왑 체인과 백 버퍼 타겟을 소유합니다. RenderSubsystem이 생성·크기 변경·Present와 GPU 동기화를 관리합니다.
- `Texture`, `Shader`, `Buffer`: GPU 리소스를 소유하는 이동 가능한 객체. `Shader`는 PSO와 루트 시그니처를 함께 보관합니다.

프레임의 시작과 종료는 엔진의 `OnPreTick` / `OnPostTick`에서 자동으로 처리합니다. 게임의 Update 또는 LateUpdate에서 렌더링을 기록합니다. 명령 할당자는 하나이며 프레임 끝에 GPU 완료를 기다립니다.

## 텍스처에 그린 뒤 창으로 확대 출력

게임 초기화 시, 엔진의 RenderSubsystem 초기화가 끝난 뒤 타겟을 생성합니다.

```cpp
auto created = rendering.CreateRenderTarget(640, 360, true);
if (!created)
{
    Engine::GetInstance().ReportError(created.error());
    return;
}
sceneTarget = std::move(*created); // 게임이 소유하는 RenderTarget 멤버
```

게임 프레임에서 다음 순서로 명령을 기록합니다.

```cpp
if (!rendering.IsFrameActive()) return;

Renderer& renderer = rendering.GetRenderer();
renderer.SetRenderTarget(sceneTarget);
renderer.Clear(ColorRGBA<float>(0.08f, 0.12f, 0.18f, 1.0f));
renderer.SetShader(sceneShader);
renderer.SetVertexBuffer(vertices, sizeof(Vertex));
renderer.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
renderer.Draw(vertexCount);

// 현재 프레임에 표시 가능한 window에만 호출합니다.
rendering.BlitToWindow(sceneTarget, window);
```

`BlitToWindow`는 내장 화면 출력 셰이더로 전체 창에 선형 보간하여 확대합니다. 별도 사각형 정점 버퍼가 필요하지 않습니다. 창 크기가 변해도 장면 타겟 해상도는 유지합니다. 화면 비율 보존과 여백 처리는 자동 적용하지 않습니다.

## 리소스 및 상태

- GPU 작업이 끝나기 전에는 사용 중인 타겟, 텍스처, 셰이더, 버퍼를 이동하거나 해제하지 않습니다. 창 타겟 참조는 한 프레임 동안만 사용합니다.
- `SetRenderTarget`과 `SetTexture`가 텍스처 상태를 추적하고 필요한 전환 명령을 기록합니다. 같은 텍스처를 동시에 출력 대상으로 쓰고 샘플링할 수 없습니다.
- `SetTexture`는 한 텍스처의 SRV 힙을 연결합니다. 다른 텍스처를 연결하면 이전 디스크립터 테이블을 다시 구성해야 합니다. 여러 텍스처 테이블이나 추가 복사 명령은 `GetCommandList()`로 직접 기록할 수 있습니다.
- 일반 `CreateTexture`는 초기 데이터를 업로드하지 않습니다. 생성 직후 내용은 정의되지 않으므로 `Renderer::Transition`과 리소스의 `GetNativeResource()`를 사용해 업로드/복사 명령을 기록한 뒤 샘플링합니다. 렌더 타겟은 그리기 전에 `Clear`합니다.
- 일반 타겟의 선택적 깊이 버퍼는 `DXGI_FORMAT_D32_FLOAT`입니다. 장면 셰이더의 PSO에는 타겟의 컬러 포맷과 깊이 포맷을 지정해야 합니다.
- `CreateShader` / `CreateComputeShader`에는 셰이더 바이트코드와 루트 시그니처를 포함한 D3D12 PSO 설명을 전달합니다. 추가 가상 인터페이스는 없습니다.
- 버퍼의 COPY_SOURCE/DEST 등 상태 전환은 호출자가 직접 기록합니다. `GetCommandList()` 사용 시 텍스처의 상태 추적을 우회하지 않습니다.

창 출력은 `Render.BufferCount` 옵션을 사용합니다. RenderSubsystem은 창별 SwapChain을 소유하며, 펜스는 내부 데이터로 유지합니다.
