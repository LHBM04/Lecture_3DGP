#if UNITY_EDITOR
using System;
using System.Collections.Generic;

using UnityEditor;
using UnityEngine;

using Object = UnityEngine.Object;

namespace TUK.Editor
{
    internal abstract class TUKExporterWindow : EditorWindow
    {
        [SerializeField] private string outputRoot = "";
        [SerializeField] private List<Object> selectedObjects = new();
        [SerializeField] private Object pendingObject;

        [SerializeField] private string vertexShaderPath = "Shaders/Standard.vert.cso";
        [SerializeField] private string hullShaderPath = "";
        [SerializeField] private string domainShaderPath = "";
        [SerializeField] private string geometryShaderPath = "";
        [SerializeField] private string pixelShaderPath = "Shaders/Standard.frag.cso";

        private Vector2 scrollPosition;
        private GUIStyle cardStyle;
        private GUIStyle headingStyle;

        protected abstract Type ObjectType 
        { 
            get; 
        }

        protected virtual bool AllowSceneObjects
        {
            get
            {
                return false;
            }
        }

        protected virtual bool ShowShaderPaths
        {
            get
            {
                return false;
            }
        }

        protected static void OpenWindow<TWindow>(string title) where TWindow : TUKExporterWindow
        {
            TWindow window = GetWindow<TWindow>(title);
            window.minSize = new Vector2(300f, 520f);
            window.AddObjects(Selection.objects);
            window.Show();
        }

        protected virtual void OnGUI()
        {
            HandleWindowDrop();

            EnsureStyles();
            DrawHeader();

            scrollPosition = EditorGUILayout.BeginScrollView(scrollPosition);
            EditorGUILayout.BeginVertical();
            GUILayout.Space(8f);

            BeginCard();
            DrawOutputPath();
            EndCard();

            if (ShowShaderPaths)
            {
                BeginCard();
                EditorGUILayout.LabelField("Shader Settings", EditorStyles.boldLabel);
                GUILayout.Space(8f);

                vertexShaderPath = DrawPath("Vertex Shader", vertexShaderPath);
                hullShaderPath = DrawPath("Hull Shader", hullShaderPath);
                domainShaderPath = DrawPath("Domain Shader", domainShaderPath);
                geometryShaderPath = DrawPath("Geometry Shader", geometryShaderPath);
                pixelShaderPath = DrawPath("Pixel Shader", pixelShaderPath);

                EndCard();
            }

            BeginCard();
            DrawObjects();
            EndCard();

            GUILayout.Space(8f);
            EditorGUILayout.EndVertical();
            EditorGUILayout.EndScrollView();

            DrawButtons();
        }

        private void EnsureStyles()
        {
            if (cardStyle != null)
            {
                return;
            }

            cardStyle = new(EditorStyles.helpBox);
            cardStyle.padding = new RectOffset(14, 14, 12, 12);
            cardStyle.margin = new RectOffset(12, 12, 0, 10);

            headingStyle = new(EditorStyles.boldLabel);
            headingStyle.fontSize = 17;
            headingStyle.alignment = TextAnchor.MiddleCenter;
        }

        private void DrawHeader()
        {
            GUILayout.Space(12f);
            EditorGUILayout.LabelField(titleContent.text, headingStyle, GUILayout.Height(28f));
            GUILayout.Space(8f);
        }

        private void DrawButtons()
        {
            GUILayout.Space(8f);
            float buttonWidth = Mathf.Max(0f, (position.width - 30f) * 0.5f);

            EditorGUILayout.BeginHorizontal();
            GUILayout.Space(12f);

            using (new EditorGUI.DisabledScope(selectedObjects.Count == 0 || string.IsNullOrWhiteSpace(outputRoot)))
            {
                Color previousColor = GUI.backgroundColor;
                GUI.backgroundColor = GUI.enabled
                    ? new Color(0.35f, 0.8f, 0.65f)
                    : new Color(0.25f, 0.25f, 0.25f);

                bool export = GUILayout.Button("Export", GUILayout.Width(buttonWidth), GUILayout.Height(32f));
                GUI.backgroundColor = previousColor;

                if (export)
                {
                    Export();
                }
            }

            GUILayout.Space(6f);

            if (GUILayout.Button("Clear", GUILayout.Width(buttonWidth), GUILayout.Height(32f)))
            {
                ClearField();
            }

            GUILayout.Space(12f);
            EditorGUILayout.EndHorizontal();
            GUILayout.Space(10f);
        }

        private void BeginCard()
        {
            EditorGUILayout.BeginVertical(cardStyle);
        }

        private static void EndCard()
        {
            EditorGUILayout.EndVertical();
        }

        private void DrawOutputPath()
        {
            EditorGUILayout.LabelField("Save Location", EditorStyles.boldLabel);
            GUILayout.Space(8f);

            EditorGUILayout.BeginHorizontal();
            outputRoot = EditorGUILayout.TextField(outputRoot, GUILayout.Height(22f));

            if (GUILayout.Button("Browse…", GUILayout.Width(82f), GUILayout.Height(22f)))
            {
                string path = EditorUtility.SaveFolderPanel("리소스 저장 경로", outputRoot, "");
                if (!string.IsNullOrEmpty(path))
                {
                    outputRoot = path;
                }
            }

            EditorGUILayout.EndHorizontal();
        }

        private static string DrawPath(string label, string value)
        {
            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField(label, GUILayout.Width(110f));
            string result = EditorGUILayout.TextField(value, GUILayout.Height(20f));
            EditorGUILayout.EndHorizontal();
            GUILayout.Space(3f);

            return result;
        }

        private void DrawObjects()
        {
            EditorGUILayout.BeginHorizontal();
            EditorGUILayout.LabelField("Selected Objects", EditorStyles.boldLabel, GUILayout.MinWidth(0f));

            if (GUILayout.Button("Add Selected", GUILayout.Width(105f), GUILayout.Height(22f)))
            {
                AddObjects(Selection.objects);
            }

            EditorGUILayout.EndHorizontal();
            GUILayout.Space(8f);

            EditorGUILayout.BeginHorizontal();
            pendingObject = EditorGUILayout.ObjectField(pendingObject, ObjectType, AllowSceneObjects);

            using (new EditorGUI.DisabledScope(!IsCompatible(pendingObject)))
            {
                if (GUILayout.Button("Add", GUILayout.Width(60f)))
                {
                    AddObject(pendingObject);
                    pendingObject = null;
                }
            }

            EditorGUILayout.EndHorizontal();
            GUILayout.Space(10f);

            for (int i = 0; i < selectedObjects.Count; i++)
            {
                Object previous = selectedObjects[i];

                EditorGUILayout.BeginHorizontal(EditorStyles.helpBox);
                GUILayout.Label((i + 1).ToString("00"), EditorStyles.centeredGreyMiniLabel, GUILayout.Width(24f));

                Texture icon = previous ? AssetPreview.GetMiniThumbnail(previous) : null;
                GUILayout.Label(icon, GUILayout.Width(20f), GUILayout.Height(20f));

                Object replacement = EditorGUILayout.ObjectField(previous, ObjectType, AllowSceneObjects);
                bool remove = GUILayout.Button(new GUIContent("×", "목록에서 제거"), GUILayout.Width(24f), GUILayout.Height(20f));
                EditorGUILayout.EndHorizontal();
                GUILayout.Space(3f);

                if (remove)
                {
                    selectedObjects.RemoveAt(i);
                    i--;
                    continue;
                }

                if (replacement != previous)
                {
                    if (!replacement || (IsCompatible(replacement) && !selectedObjects.Contains(replacement)))
                    {
                        selectedObjects[i] = replacement;
                    }
                }
            }
        }

        private void HandleWindowDrop()
        {
            Event current = Event.current;
            if (current.type != EventType.DragUpdated && current.type != EventType.DragPerform)
            {
                return;
            }

            Rect windowArea = new(0f, 0f, position.width, position.height);
            if (!windowArea.Contains(current.mousePosition))
            {
                return;
            }

            bool accepted = false;
            foreach (Object candidate in DragAndDrop.objectReferences)
            {
                if (IsCompatible(candidate))
                {
                    accepted = true;
                    break;
                }
            }

            DragAndDrop.visualMode = accepted ? DragAndDropVisualMode.Copy : DragAndDropVisualMode.Rejected;
            if (accepted && current.type == EventType.DragPerform)
            {
                DragAndDrop.AcceptDrag();
                AddObjects(DragAndDrop.objectReferences);
            }

            current.Use();
        }

        private bool IsCompatible(Object value)
        {
            return value && ObjectType.IsInstanceOfType(value) &&
                (AllowSceneObjects || EditorUtility.IsPersistent(value));
        }

        private void AddObject(Object value)
        {
            if (IsCompatible(value) && !selectedObjects.Contains(value))
            {
                selectedObjects.Add(value);
            }
        }

        private void AddObjects(IEnumerable<Object> objects)
        {
            foreach (Object value in objects)
            {
                AddObject(value);
            }

            Repaint();
        }

        private void ClearField()
        {
            selectedObjects.Clear();
            pendingObject = null;
            outputRoot = "";

            vertexShaderPath = "";
            hullShaderPath = "";
            domainShaderPath = "";
            geometryShaderPath = "";
            pixelShaderPath = "";
        }

        private ShaderData GetShaderData()
        {
            ShaderData data = new();

            data.VertexShader = vertexShaderPath.Trim().Replace('\\', '/');
            data.HullShader = hullShaderPath.Trim().Replace('\\', '/');
            data.DomainShader = domainShaderPath.Trim().Replace('\\', '/');
            data.GeometryShader = geometryShaderPath.Trim().Replace('\\', '/');
            data.PixelShader = pixelShaderPath.Trim().Replace('\\', '/');

            return data;
        }

        private void Export()
        {
            selectedObjects.RemoveAll(value => !IsCompatible(value));
            if (selectedObjects.Count == 0)
            {
                Debug.LogWarning($"내보낼 {ObjectType.Name}을 창에 추가하세요.");
                return;
            }

            if (string.IsNullOrWhiteSpace(outputRoot))
            {
                Debug.LogWarning("저장할 루트 폴더를 지정하세요.");
                return;
            }

            TUKExporter.Export(selectedObjects.ToArray(), outputRoot.Trim(), GetShaderData());
        }
    }
}
#endif
