using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal struct AnimationFrameData
    {
        internal float Time;
        internal Vector3 Position;
        internal Quaternion Rotation;
        internal Vector3 Scale;
    }

    internal struct AnimationTransformData
    {
        internal string Path;
        internal List<AnimationFrameData> Frames;
    }

    internal struct AnimationData
    {
        internal string Name;
        internal float Length;
        internal float FrameRate;
        internal List<AnimationTransformData> Transforms;
    }

    internal static class TUKAnimationExporter
    {
        internal static readonly string DirectoryPath = "Resources/Animations/";
        internal static readonly string FileExtension = ".tukanim";

        private sealed class TransformCurves
        {
            internal readonly AnimationCurve[] Position = new AnimationCurve[3];
            internal readonly AnimationCurve[] Rotation = new AnimationCurve[4];
            internal readonly AnimationCurve[] Euler = new AnimationCurve[3];
            internal readonly AnimationCurve[] Scale = new AnimationCurve[3];
        }

        internal static void Save(string path, AnimationData data)
        {
            using (TUKBinaryWriter writer = new(path))
            {
                Save(writer, data);
            }
        }

        internal static void Save(TUKBinaryWriter writer, AnimationData data)
        {
            writer.Write(data.Name);
            writer.Write(data.Length);
            writer.Write(data.FrameRate);

            writer.Write(data.Transforms.Count);
            foreach (AnimationTransformData transform in data.Transforms)
            {
                writer.Write(transform.Path);

                writer.Write(transform.Frames.Count);
                foreach (AnimationFrameData frame in transform.Frames)
                {
                    writer.Write(frame.Time);
                    writer.Write(frame.Position);
                    writer.Write(frame.Rotation);
                    writer.Write(frame.Scale);
                }
            }
        }

        internal static AnimationData GetMetaData(AnimationClip current)
        {
            if (!current)
            {
                return default;
            }

            string sourcePath = AssetDatabase.GetAssetPath(current);
            GameObject model = AssetDatabase.LoadAssetAtPath<GameObject>(sourcePath);

            return GetMetaData(current, model ? model.transform : null);
        }

        internal static AnimationData GetMetaData(AnimationClip current, Transform referenceRoot)
        {
            if (!current)
            {
                return default;
            }

            if (current.humanMotion)
            {
                Debug.LogWarning("Humanoid 클립은 Generic Transform 애니메이션으로 베이크한 뒤 내보내세요.");
                return default;
            }

            Dictionary<string, TransformCurves> curvesByPath = new(StringComparer.Ordinal);
            foreach (EditorCurveBinding binding in AnimationUtility.GetCurveBindings(current))
            {
                if (!typeof(Transform).IsAssignableFrom(binding.type))
                {
                    continue;
                }

                string property = binding.propertyName;
                int axis = "xyzw".IndexOf(property[property.Length - 1]);
                if (axis < 0)
                {
                    continue;
                }

                bool position = property.StartsWith("m_LocalPosition.", StringComparison.Ordinal) && axis < 3;
                bool rotation = property.StartsWith("m_LocalRotation.", StringComparison.Ordinal);
                bool euler = (property.StartsWith("localEulerAnglesRaw.", StringComparison.Ordinal) ||
                    property.StartsWith("localEulerAnglesBaked.", StringComparison.Ordinal) ||
                    property.StartsWith("localEulerAngles.", StringComparison.Ordinal)) && axis < 3;
                bool scale = property.StartsWith("m_LocalScale.", StringComparison.Ordinal) && axis < 3;

                if (!position && !rotation && !euler && !scale)
                {
                    continue;
                }

                if (!curvesByPath.TryGetValue(binding.path, out TransformCurves curves))
                {
                    curves = new();
                    curvesByPath.Add(binding.path, curves);
                }

                AnimationCurve curve = AnimationUtility.GetEditorCurve(current, binding);
                if (position)
                {
                    curves.Position[axis] = curve;
                }
                else if (rotation)
                {
                    curves.Rotation[axis] = curve;
                }
                else if (euler)
                {
                    curves.Euler[axis] = curve;
                }
                else
                {
                    curves.Scale[axis] = curve;
                }
            }

            if (curvesByPath.Count == 0)
            {
                Debug.LogWarning($"'{current.name}'에 내보낼 TRS 애니메이션이 없습니다.");
                return default;
            }

            AnimationData data = new();
            data.Name = current.name;
            data.Length = current.length;
            data.FrameRate = current.frameRate > 0f ? current.frameRate : 30f;
            data.Transforms = new();

            int intervals = Mathf.CeilToInt(data.Length * data.FrameRate);

            foreach (var entry in curvesByPath.OrderBy(value => value.Key, StringComparer.Ordinal))
            {
                Transform reference = referenceRoot;
                if (referenceRoot && !string.IsNullOrEmpty(entry.Key))
                {
                    reference = referenceRoot.Find(entry.Key);
                }

                Vector3 basePosition = reference ? reference.localPosition : Vector3.zero;
                Quaternion baseRotation = reference ? reference.localRotation : Quaternion.identity;
                Vector3 baseScale = reference ? reference.localScale : Vector3.one;
                TransformCurves curves = entry.Value;

                AnimationTransformData transform = new();
                transform.Path = entry.Key;
                transform.Frames = new();

                Quaternion previousRotation = baseRotation;

                for (int i = 0; i <= intervals; i++)
                {
                    float time = Mathf.Min(i / data.FrameRate, data.Length);
                    Quaternion rotation = baseRotation;

                    if (curves.Rotation.Any(value => value != null))
                    {
                        for (int axis = 0; axis < 4; axis++)
                        {
                            if (curves.Rotation[axis] != null)
                            {
                                rotation[axis] = curves.Rotation[axis].Evaluate(time);
                            }
                        }

                        rotation = rotation.normalized;
                    }
                    else if (curves.Euler.Any(value => value != null))
                    {
                        rotation = Quaternion.Euler(Evaluate(curves.Euler, baseRotation.eulerAngles, time));
                    }

                    if (Quaternion.Dot(previousRotation, rotation) < 0f)
                    {
                        rotation = new Quaternion(-rotation.x, -rotation.y, -rotation.z, -rotation.w);
                    }

                    AnimationFrameData frame = new();
                    frame.Time = time;
                    frame.Position = Evaluate(curves.Position, basePosition, time);
                    frame.Rotation = rotation;
                    frame.Scale = Evaluate(curves.Scale, baseScale, time);

                    transform.Frames.Add(frame);
                    previousRotation = rotation;
                }

                data.Transforms.Add(transform);
            }

            return data;
        }

        private static Vector3 Evaluate(AnimationCurve[] curves, Vector3 value, float time)
        {
            for (int axis = 0; axis < 3; axis++)
            {
                if (curves[axis] != null)
                {
                    value[axis] = curves[axis].Evaluate(time);
                }
            }

            return value;
        }
    }
}
