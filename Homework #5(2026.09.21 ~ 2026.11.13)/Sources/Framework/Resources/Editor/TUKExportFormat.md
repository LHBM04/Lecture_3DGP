# TUK.Editor 순서 기반 저장 규약

모델 익스포터를 기준으로 각 타입의 GetMetaData, Save(serializer, data), 메뉴 Export를 구성한다.
Save(path, data)는 파일별 serializer를 열고 닫는다. 모델 노드의 재귀 기록은 같은 serializer를 공유한다.
머티리얼의 Save(path, data)는 텍스처를 먼저 복사하며, 폰트는 아틀라스를 먼저 저장한다.
직접 Save(serializer, data)를 호출할 때는 SaveTextures(root, data) 또는 SaveAtlases(directory, data)를 먼저 호출해야 한다.

## 공통

헤더와 필드 ID 없이 아래 순서대로 기록한다. .NET BinaryWriter의 little-endian int32, uint32,
float32, 1바이트 bool을 사용한다. 문자열은 BinaryWriter.Write(string)의 UTF-8 및 7-bit 길이 인코딩이다.
벡터는 x/y/z/w, Color는 r/g/b/a 순서다. 개수는 int32다.
이 규약은 기존 C++ TUKR 블록 포맷 및 기존 바이너리 리소스와 호환되지 않는다.
Unity 컴파일, 내보내기 실행, C++ 로더 연동은 별도 확인이 필요하다.

## 모델

이름, Vector3 위치, Quaternion 회전, Vector3 크기, 메시 경로, 머티리얼 경로,
자식 개수, 각 자식의 동일한 노드 데이터를 순서대로 기록한다.
출력 루트를 선택하면 Resources/Models, Meshes, Materials, Textures 아래에 저장한다.
의존 파일 경로는 출력 루트 기준 Resources/...이다.
현재 모델 서식은 노드당 메시 하나와 머티리얼 하나다. 여러 머티리얼 슬롯은 거부한다.
파일 이름은 원본 에셋명을 사용한다. 같은 폴더에 동일한 이름이 있으면 덮어쓴다.

## 메시

Position 개수 및 Vector3들, Normal 개수 및 Vector3들,
TexCoord 개수 및 Vector2들, Indicies 개수 및 int32들을 기록한다.
UV Y는 뒤집지 않는다. Read/Write가 켜진 삼각형 메시를 지원한다.
서브메시 인덱스는 하나의 삼각형 목록으로 합쳐지며 별도 범위는 저장하지 않는다.

## 머티리얼

BaseColor, EmissiveColor, Metallic, Roughness,
BaseColorTexture, NormalTexture, MetallicTexture, RoughnessTexture, OcclusionTexture,
EmissiveTexture, HeightTexture, SpecularTexture, MaskTexture,
VertexShader, HullShader, DomainShader, GeometryShader, PixelShader 순서다.
선택 속성이 없으면 기본 색상, Metallic=0, Roughness=0.5 또는 빈 문자열이다.
Smoothness/Glossiness는 1-value로 Roughness에 변환한다.
셰이더는 기존 Standard CSO 경로를 기본으로 지정하며 Unity 셰이더를 HLSL로 변환하지 않는다.
텍스처는 원본 이미지 그대로 복사한다. DDS 변환은 하지 않는다.
기존 C++ DDS 전용 로더에 사용하려면 이미지 변환과 메타데이터 경로 변경이 필요하다.
텍스처별 offset/scale, Unity 셰이더의 모든 옵션은 이 모델 서식에 포함하지 않는다.

## TMP 폰트

Name(string), RenderMode(string), IsDistanceField(bool), PointSize(float), FaceScale(float),
LineHeight(float), AscentLine(float), DescentLine(float), AtlasPadding(int), 아틀라스 개수 및 아틀라스들,
문자 개수 및 문자들을 기록한다.
아틀라스는 Index(int), TexturePath(string), Width(int), Height(int) 순서다.
문자는 Unicode(uint), GlyphIndex(uint), AtlasIndex(int), CharacterScale(float), GlyphScale(float),
GlyphRect(Vector4), SampleRect(Vector4), UVMin(Vector2), UVMax(Vector2),
Width(float), Height(float), BearingX(float), BearingY(float), Advance(float) 순서다.
Rect는 x/y/width/height 순서이며 float32다. 좌표와 UV는 좌측 상단 원점이다.
SDF는 원본 padding을 샘플 영역에 포함한다. 폰트 PNG는 R과 Alpha 모두 같은 선형 마스크다.
아틀라스 경로는 폰트 메타 파일 기준이다. 생성된 TMP 문자만 내보내며 커닝, 폴백, 동적 생성은 포함하지 않는다.
Alpha8/R8 아틀라스를 지원하며 메타데이터 수집 시 PNG 바이트도 준비한다.

## 애니메이션 (.tukanim)

클립 이름(string), 길이(float), 샘플링 프레임 레이트(float), Transform 개수(int)를 기록한다.
각 Transform은 루트 기준 경로(string), 프레임 개수(int), 프레임들의 순서다.
프레임은 Time(float), Position(Vector3), Rotation(Quaternion x/y/z/w), Scale(Vector3)만 기록한다.
클립의 frameRate 간격으로 샘플링하며 마지막에는 클립 끝 시간을 포함한다. 길이가 0이면 시간 0의 프레임 하나를 기록한다.
커브, 탄젠트, 객체 참조, 이벤트, 루프 설정은 파일에 기록하지 않는다.
Transform 로컬 TRS만 사용하며 Generic 클립을 지원한다. Humanoid 클립은 먼저 베이크해야 한다.
같은 FBX의 원본 모델을 찾으면 키가 없는 축의 기본 TRS로 사용한다.
독립 .anim 등 원본 모델을 찾지 못하면 기본 위치 0, 회전 Identity, 크기 1을 사용한다.
다른 기준 모델이 필요한 호출자는 GetMetaData(clip, referenceRoot) 오버로드로 지정할 수 있다.
기존 .tukanim과 저장 규칙이 달라 재수출 및 C++ 로더 대응이 필요하다.

메타데이터는 일반 struct와 필드를 사용하며 생성 후 각 필드에 직접 대입하여 초기화한다.

## 큐브맵

TUKCubemapExporter는 선택한 Cubemap을 GetMetaData에서 읽어 6면의 PNG 데이터를 준비한다.
Save(directory, data)는 준비된 이미지를 저장한다. 바이너리 메타 파일은 만들지 않는다.
출력은 선택한 루트/Resources/Cubemaps/에셋명/ 아래의 PNG 6개다.
면 순서는 PositiveX, NegativeX, PositiveY, NegativeY, PositiveZ, NegativeZ이며 원본 픽셀 방향을 유지한다.
기존처럼 RGBA8 PNG로 변환하므로 HDR 값과 밉맵은 보존하지 않는다. DDS 큐브맵 변환은 별도 작업이다.

출력 파일명에는 GUID, 로컬 ID, 인스턴스 ID를 붙이지 않는다. 모델은 GameObject 이름, 나머지는 원본 에셋 이름을 사용한다.
폰트 아틀라스는 페이지 번호, 큐브맵 이미지는 면 이름을 구분 접미사로 사용한다.

텍스처 출력은 원본 경로의 Textures/ 아래 폴더 구조를 유지한다.
예: Assets/WW2 Armed Forces Demo/Textures/Carro_Armato_L6_40/Body.png → Resources/Textures/Carro_Armato_L6_40/Body.png.
Textures 하위 폴더를 찾을 수 없으면 머티리얼 이름의 폴더에 저장한다. 실제 복사 경로와 머티리얼의 참조 경로는 동일한 값을 사용한다.

## 내보내기 창

Tools > TUK > Exporter에서 창을 연다. Export/Clear, 5개 셰이더 경로, 현재 선택 목록 순서다.
Export는 선택 에셋의 기존 익스포터를 호출하고 모델의 머티리얼에도 같은 셰이더 설정을 전달한다.
CSO 경로는 텍스트로 지정하며 실제 CSO 파일을 복사하거나 Unity Shader를 변환하지 않는다.
Clear는 경로와 Unity 선택을 비운다. 개별 Export 메뉴는 기존 기본 셰이더를 사용한다.

Window는 선택 표시, 셰이더 입력과 폴더 대화상자만 담당한다. TUKExporter.Export는 전달받은 객체 목록과 출력 루트 및 ShaderData로 검증·타입별 내보내기·중복 부모 처리를 수행한다. ShaderData는 별도 파일로 관리한다.

## 타입별 내보내기 창

Tools/TUK/Export Model, Mesh, Material, Font, Animation, Cubemap은 각각 전용 Window를 연다.
TUKExporterWindow는 공통 UI 기반 클래스이며 별도 메뉴는 없다. 저장 루트는 창의 Save Path에 입력하거나 Browse로 지정한다. Export는 추가 대화상자 없이 이 경로를 사용한다.
창별 목록은 드래그 앤 드롭, 오브젝트 선택 필드와 Add, Unity 현재 선택의 Add Selected로 구성한다. 각 창에 맞는 타입만 추가하며 중복은 제외한다. Remove로 개별 삭제할 수 있다.
Clear는 해당 창의 목록, 경로와 셰이더 입력을 비우며 Unity의 현재 선택은 변경하지 않는다. 셰이더 경로는 Model/Material 창에만 표시한다.
각 Exporter는 수집과 저장만 담당하고 메뉴 및 폴더 선택 코드는 Window로 이동했다.

각 Window는 별도 드롭 박스 없이 내용 영역 전체에서 드래그 앤 드롭을 받는다. 기존 목록 위에 놓아도 교체하지 않고 새 항목으로 추가하며 중복은 제외한다.
