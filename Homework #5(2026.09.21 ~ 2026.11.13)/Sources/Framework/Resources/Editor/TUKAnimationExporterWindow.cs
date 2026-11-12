using System;

using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal sealed class TUKAnimationExporterWindow : TUKExporterWindow
    {
        protected override Type ObjectType
        {
            get
            {
                return typeof(AnimationClip);
            }
        }

        [MenuItem("Tools/TUK/Export Animation")]
        private static void Open()
        {
            OpenWindow<TUKAnimationExporterWindow>("TUK Animation Exporter");
        }
    }
}
