# MyGameEngine 仕様書

## 1. 概要

MyGameEngine は、Windows 10/11 上で動作する C++20 / DirectX 12 ベースの学習用3Dゲームエンジンである。
Win32アプリケーション、Scene / GameObject / Component、入力、時間管理、リソース管理、および基本的な3D描画機能を提供する。

本仕様書は現在のリポジトリに実装されている機能を対象とする。

## 2. 動作環境

- OS: Windows 10 または Windows 11
- アーキテクチャ: x64
- IDE: Visual Studio 2022
- Platform Toolset: v143
- 言語規格: C++20
- Graphics API: Direct3D 12
- 必要なWindows SDK: Windows 10/11 SDK
- ビルド構成: Debug / Release

リンクライブラリ:

- `d3d12.lib`
- `dxgi.lib`
- `d3dcompiler.lib`
- `windowscodecs.lib`

## 3. ディレクトリ構成

```text
MyGameEngine/
├─ assets/
│  ├─ models/             OBJモデル
│  ├─ shaders/            HLSLシェーダー
│  └─ textures/           テクスチャ
├─ src/
│  ├─ main.cpp            Win32エントリーポイント
│  └─ Engine/
│     ├─ Core/             Application、Engine、TimeSystem
│     ├─ Graphics/         Renderer、Camera、Mesh、Material、RenderQueue
│     ├─ Input/            キーボード入力
│     ├─ Platform/         Win32 Window
│     ├─ Resource/         Texture、ResourceManager
│     └─ Scene/            Scene、GameObject、Transform、Component
├─ MyGameEngine.sln
└─ MyGameEngine.vcxproj
```

## 4. アプリケーション構成

### 4.1 エントリーポイント

`wWinMain` が `Application` を生成し、`Application::Run` を呼び出す。

### 4.2 Application

`Application` は次の責務を持つ。

- `Engine` の生成、初期化、実行、終了
- 初期Sceneの構築
- 描画サンプル用GameObject、Mesh、Materialの生成

現在のサンプルSceneには、三角形2個とQuad 1個の計3個の描画Objectが配置される。
三角形は `assets/models/triangle.obj` の読み込みを試み、失敗した場合は組み込み三角形を使用する。

### 4.3 Engine

`Engine` は各サブシステムの所有者であり、以下を管理する。

- `Window`
- `Renderer`
- `InputSystem`
- `TimeSystem`
- `SceneManager`
- `ResourceManager`
- `Camera`
- `RenderQueue`

初期化順序:

1. 入力、時間、Scene、Resourceサブシステム生成
2. 1280 × 720のWin32 Window生成
3. Renderer初期化
4. Camera生成およびPerspective設定
5. RenderQueue生成

終了時はGPU処理の完了を待ち、描画リソースをWindowより先に解放する。

## 5. メインループ

EngineはWin32メッセージを処理した後、各フレームで次の順序を実行する。

```text
Win32 Message Processing
        ↓
TimeSystem::Update
        ↓
InputSystem::Update
        ↓
SceneManager::Update
        ↓
RenderQueue::Collect
        ↓
Renderer::Render
        ↓
ExecuteCommandLists
        ↓
Present
        ↓
Fence Wait
```

`WM_QUIT` を受信するとループを終了する。描画失敗時は終了コード1での終了を要求する。

## 6. Window仕様

- Win32 APIを使用する。
- 初期クライアントサイズは1280 × 720。
- ウィンドウスタイルは `WS_OVERLAPPEDWINDOW`。
- `WM_DESTROY` 受信時に `PostQuitMessage` を呼ぶ。
- DXGIのAlt+Enterによる暗黙的フルスクリーン切り替えは無効。
- 現在、ウィンドウリサイズに伴うSwapChain再生成は未実装。

## 7. Renderer仕様

### 7.1 DeviceとAdapter

- Debug構成ではD3D12 Debug Layerを有効化する。
- `IDXGIFactory6` を使用する。
- `DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE` の順にAdapterを検索する。
- Software Adapterを通常の選択対象から除外する。
- 必要Feature Levelは `D3D_FEATURE_LEVEL_12_0`。
- 対応ハードウェアがない場合はWARP Adapterへフォールバックする。

### 7.2 SwapChain

- Swap Effect: `DXGI_SWAP_EFFECT_FLIP_DISCARD`
- Buffer Count: 2
- BackBuffer Format: `DXGI_FORMAT_R8G8B8A8_UNORM`
- Sample Count: 1
- Buffer Usage: `DXGI_USAGE_RENDER_TARGET_OUTPUT`
- PresentはVSync有効時にSync Interval 1を使用する。

### 7.3 Render Target

- RTV Descriptor HeapにBackBuffer数と同数のDescriptorを確保する。
- 現在のBackBuffer Indexは `IDXGISwapChain3` から取得する。
- Clear ColorはRGBA `(0.04, 0.07, 0.12, 1.0)`。

### 7.4 Command

- Command Queue Type: Direct
- Command Allocator: 1個
- Graphics Command List: 1個
- 毎フレームCommand AllocatorとCommand ListをResetして再利用する。

### 7.5 Resource State Transition

フレーム開始時:

```text
PRESENT → RENDER_TARGET
```

フレーム終了時:

```text
RENDER_TARGET → PRESENT
```

### 7.6 GPU同期

- `ID3D12Fence` とWin32 Eventを使用する。
- Command実行およびPresent後にFenceをSignalする。
- CPUはFence完了まで待機してから次フレームへ進む。
- 現在は安全性を優先した1フレーム単位の完全同期方式であり、複数フレームの並列処理は行わない。

## 8. Shader / Pipeline仕様

### 8.1 Shader

- Shader Model: 5.0
- Vertex Shader Entry Point: `VSMain`
- Pixel Shader Entry Point: `PSMain`
- `D3DCompile` により実行時コンパイルする。
- Debug構成ではDebug情報を付与し、最適化を無効化する。
- Shaderソースの参照版は `assets/shaders/Default.hlsl` に配置する。

現在のRendererは起動時のパス依存を避けるため、同等の標準ShaderソースをRenderer内からコンパイルする。

### 8.2 Vertex Input

| Semantic | Format | Offset |
|---|---|---:|
| POSITION | R32G32B32_FLOAT | 0 |
| NORMAL | R32G32B32_FLOAT | 12 |
| TEXCOORD | R32G32_FLOAT | 24 |

### 8.3 Root Signature

- Root Parameter 0: `b0` Constant Buffer View
- Root Parameter 1: `t0` Shader Resource View Descriptor Table
- Static Sampler: `s0`
- Input Assemblerを有効化する。

### 8.4 Pipeline State

- Primitive Topology: Triangle List
- Fill Mode: Solid
- Cull Mode: None
- Depth Clip: Enabled
- Depth / Stencil: Disabled
- Alpha Blending: Disabled
- Render Target Count: 1

## 9. Mesh仕様

### 9.1 Vertex

Vertexは次のデータを持つ。

- Position: `XMFLOAT3`
- Normal: `XMFLOAT3`
- Texture Coordinate: `XMFLOAT2`

### 9.2 Buffer

- Vertex BufferおよびIndex Bufferを保持する。
- Index Formatは `DXGI_FORMAT_R32_UINT`。
- 現在は実装の単純化のためUpload Heap上にBufferを生成する。
- GPU Bufferは最初に描画される際に遅延生成する。

### 9.3 組み込みMesh

- Triangle
- Quad

## 10. OBJローダー仕様

対応要素:

- `v`: 頂点座標
- `vn`: 法線
- `vt`: UV座標
- `f`: Face
- `v/vt/vn` 形式のIndex
- 3頂点を超えるFaceのTriangle Fanによる三角形化
- 同一Index組み合わせのVertex共有
- UVのV座標反転

制限事項:

- 正のIndexのみ対応する。
- Material Library (`.mtl`) は未対応。
- Smoothing Group、Group、Object名は使用しない。
- 法線がない場合は既定法線 `(0, 0, -1)` を使用する。

## 11. Transform / Camera / Constant Buffer

### 11.1 Transform

GameObjectごとに以下を保持する。

- Position
- Rotation（Pitch / Yaw / Roll、ラジアン）
- Scale

World Matrix:

```text
Scale × Rotation × Translation
```

### 11.2 Camera

- Left-Handed座標系を使用する。
- View Matrixは `XMMatrixLookAtLH` で生成する。
- Projection Matrixは `XMMatrixPerspectiveFovLH` で生成する。
- 初期位置: `(0, 0, -5)`
- 初期注視点: `(0, 0, 0)`
- FOV: 45度
- Aspect Ratio: 1280 / 720
- Near / Far: 0.1 / 100.0

### 11.3 Object Constant Buffer

描画Objectごとに以下を転送する。

- World View Projection Matrix
- World Matrix
- Material Color
- Directional Light方向

Constant Bufferサイズは256 byte境界に切り上げる。現在は各Object・各フレームでUpload Bufferを生成し、Fence完了まで保持する。

## 12. Texture仕様

### 12.1 ファイル読み込み

- Windows Imaging Component (WIC)を使用する。
- 読み込み結果はRGBA 32 bitへ変換する。
- CPU側にWidth、Height、Pixel Dataを保持する。
- ResourceManagerがファイルパス単位でTextureをキャッシュする。

WIC Decoderが対応する代表的な形式としてPNG、JPEG、BMP、TIFF、GIFなどを利用できる。

### 12.2 GPU Texture

- Rendererは2 × 2の既定Checker Textureを生成する。
- Default HeapへTextureを生成し、Upload Bufferからコピーする。
- コピー後、`PIXEL_SHADER_RESOURCE` StateへTransitionする。
- Shader VisibleなCBV/SRV/UAV HeapにSRVを作成する。
- Linear Filter / Wrap Address ModeのStatic Samplerを使用する。

現在、Materialに設定した個別TextureをGPUへUploadしてDescriptorを切り替える処理は未実装であり、描画時は共通のChecker Textureを使用する。

## 13. Material仕様

Materialは次のパラメーターを持つ。

- RGBA Color
- Textureへの共有参照

Material ColorはObject Constant Bufferを介してPixel Shaderへ転送される。

## 14. Lighting仕様

- Light Type: Directional Light
- Diffuse Model: Lambert
- Ambient係数: 0.2
- Diffuse係数: 0.8
- Light Direction: `(0.3, -0.7, 1.0)`

最終色:

```text
TextureColor × MaterialColor × (Ambient + Diffuse)
```

## 15. Scene仕様

### 15.1 Scene

- 複数のGameObjectを `unique_ptr` で所有する。
- GameObjectの生成、削除、更新、全削除を提供する。
- RenderQueue構築用にGameObject一覧の読み取りアクセスを提供する。

### 15.2 GameObject

- `Transform` を1個所有する。
- 任意個数のComponentを `unique_ptr` で所有する。
- Component追加と型指定取得を提供する。
- Copy / Moveは禁止する。

### 15.3 Component

- `Update(float deltaTime)` を持つ基底クラス。
- 現在の具象Component:
  - `MoveComponent`
  - `MeshRenderer`

### 15.4 MeshRenderer

MeshRendererは以下を保持する。

- 所有GameObjectへの参照
- Meshへの共有参照
- Materialへの共有参照

## 16. Render Queue仕様

各フレームで現在のSceneを走査し、`MeshRenderer` を持つGameObjectを描画対象として収集する。
現在は登録順に描画し、Sorting、Culling、Batching、透明Objectの分離は行わない。

## 17. ResourceManager仕様

- Textureを正規化したファイルパス文字列で管理する。
- 同一パスの重複読み込み時は既存の共有Textureを返す。
- 個別Unloadと全Clearを提供する。
- 現在はTextureのみを管理対象とする。

## 18. Input仕様

- `GetAsyncKeyState` により256個のVirtual Key状態を毎フレーム取得する。
- 次の問い合わせを提供する。
  - Key Down
  - Key Pressed
  - Key Released
- 現在、Mouse、Gamepad、Text Inputは未対応。

## 19. Time仕様

`std::chrono::steady_clock` を使用して次を管理する。

- Delta Time（秒、`float`）
- Total Time（秒、`double`）

## 20. 所有権とライフタイム

- Engineサブシステム: `unique_ptr`
- Scene内GameObject: `unique_ptr`
- GameObject内Component: `unique_ptr`
- 共有可能なMesh、Material、Texture: `shared_ptr`
- COMオブジェクト: `Microsoft::WRL::ComPtr`
- MeshRendererのOwner: 非所有参照

## 21. エラー処理

- 初期化関数および描画関数は成功時 `true`、失敗時 `false` を返す。
- Renderer初期化途中で失敗した場合、生成済みリソースを解放する。
- Engine初期化失敗時のプロセス終了コードは1。
- OBJ読み込み失敗時、サンプルSceneは組み込みTriangleへフォールバックする。
- Texture読み込み失敗時、Textureは未ロード状態となる。
- 例外ベースのエラー通知や詳細ログシステムは現在未実装。

## 22. ビルドと実行

1. Visual Studio 2022で `MyGameEngine.sln` を開く。
2. `Debug|x64` または `Release|x64` を選択する。
3. `MyGameEngine` をビルドして実行する。

モデルとShader Assetはビルド時に出力ディレクトリへコピーされる。

## 23. 現在の制約と拡張候補

- Window Resize / SwapChain Resize未対応
- Depth Buffer未実装
- Frame Resource多重化未実装
- GPUごとの非同期Frame処理未実装
- 個別Material TextureのGPU Descriptor管理未実装
- Mipmap生成未実装
- Shader Hot Reload未実装
- `.mtl`、glTF、FBX未対応
- Frustum Culling、Occlusion Culling未実装
- Transparent Rendering、Shadow、PBR未実装
- ECS、Physics、Audio、UI未実装
- 詳細なログおよびGPU Device Lost復旧未実装

これらは現行の基本描画パイプラインを維持したまま段階的に追加できる。
