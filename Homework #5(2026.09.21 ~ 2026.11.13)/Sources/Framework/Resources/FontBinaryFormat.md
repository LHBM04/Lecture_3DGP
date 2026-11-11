# TUK 폰트 바이너리 포맷

폰트는 `.tukfont` 확장자와 공통 TUKR v1 헤더를 사용한다. 리소스 종류는 `4`이다.
정수, 실수, 문자열, 객체, 필드와 배열의 인코딩은 `BinaryResourceFormat.md`와 동일하다.

## 루트 객체

| 필드 ID | 타입 | 내용 |
| --- | --- | --- |
| 1 | u32 | 원본 형식: 1=BMFont, 2=TextMesh Pro |
| 2 | BitmapFontMetadata 객체 | BMFont일 때 존재 |
| 3 | TMPFontMetadata 객체 | TMP일 때 존재 |

원본 형식에 해당하는 객체를 사용한다. BMFont와 TMP의 메트릭은 서로 변환하지 않고 보존한다.
BMFont의 yoffset은 라인 상단 기준이고 TMP의 bearingY는 기준선 기준이므로 같은 값으로 취급하지 않는다.
페이지/아틀라스 텍스처 경로는 **폰트 파일의 디렉터리 기준**이다.

## BMFont 필드

BMFont 객체의 필드 1~5는 info, common, pages 배열, chars 배열, kernings 배열이다.
하위 객체의 필드 ID는 아래 나열한 순서대로 1부터 부여한다. 모든 필드는 필수이다.

- info: face(string), size(i32), bold(i32), italic(i32), charset(string), unicode(i32),
  stretchH(i32), smooth(i32), aa(i32), padding(i32×4), spacing(i32×2), outline(i32)
- common: lineHeight, base, scaleW, scaleH, pages, packed, alphaChnl, redChnl, greenChnl, blueChnl (모두 i32)
- page: id(i32), file(string)
- char: id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl (모두 i32)
- kerning: first, second, amount (모두 i32)

채널 선택 정보와 음수 오프셋, 커닝을 원본 그대로 보존한다.
배열 인덱스와 페이지 ID는 별개이다.

## TMP 필드

TMP 객체의 필드 1~15는 다음 순서이다. 모든 필드는 필수이다.

1. version(u32)
2. name(string)
3. renderMode(string)
4. isDistanceField(u32, 0 또는 1)
5. maskChannel(string)
6. textureColorSpace(string)
7. coordinateOrigin(string)
8. pointSize(f32)
9. faceScale(f32)
10. lineHeight(f32)
11. ascentLine(f32)
12. descentLine(f32)
13. atlasPadding(i32)
14. atlases(객체 배열)
15. characters(객체 배열)

TMP 하위 객체의 필드도 아래 순서대로 1부터 부여한다.

- atlas: index(i32), texture(string), width(i32), height(i32)
- character: unicode(u32), glyphIndex(u32), atlasIndex(i32), characterScale(f32), glyphScale(f32),
  glyphRect(객체), sampleRect(객체), uvMin(f32×2), uvMax(f32×2),
  width(f32), height(f32), bearingX(f32), bearingY(f32), advance(f32)
- rect: x(i32), y(i32), width(i32), height(i32)

## 읽기와 변환

C++ 필드 선언은 `FontMetadata.hpp`에 있다. 별도 바이너리 읽기 유틸리티 없이 다음처럼 읽는다.

```cpp
auto font = Serializer::Load<FontMetadata>(
    path, static_cast<u32>(ResourceType::FONT));
```

현재 C++에는 폰트 Asset 로더나 텍스트 렌더러가 없으므로 이번 변경은 저장 스키마,
익스포터, 기존 파일 변환과 배포 설정까지 제공한다.
추후 로더는 format에 맞는 메트릭으로 레이아웃을 처리해야 한다.

기존 BMFont JSON과 이전 TUK TMP JSON은 프로젝트 폴더에서 다음 명령으로 변환한다.

```powershell
node ./Tools/ConvertFontsToBinary.cjs
```

도구는 자신의 위치를 기준으로 Fonts 디렉터리를 찾는다. 첫 번째 인수로 다른 Fonts 디렉터리를 지정할 수 있다.
별도 패키지 없이 Node.js 내장 모듈만 사용한다. 원본 JSON과 아틀라스는 보존하고,
같은 이름의 `.tukfont`는 재생성한다. 매핑되지 않은 필드가 있으면 실패시켜 데이터 누락을 막는다.

Unity에서는 `TUKFontExporter.cs`와 `TUKSerializer.cs`를 함께 사용한다.
익스포터의 PNG 출력과 아틀라스 채널 및 좌표 규칙은 기존과 같다.
Unity 출력 PNG를 게임에서 사용하려면 기존 DDS 변환 과정이 필요하다.
현재 프로젝트의 `.tukfont`와 DDS 폰트 아틀라스는 빌드 출력 복사 대상에 포함된다.
