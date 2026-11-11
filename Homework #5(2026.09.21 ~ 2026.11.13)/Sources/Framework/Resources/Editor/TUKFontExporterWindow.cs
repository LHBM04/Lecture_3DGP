#if UNITY_EDITOR
using System;

using TMPro;
using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal sealed class TUKFontExporterWindow : TUKExporterWindow
    {
        protected override Type ObjectType
        {
            get
            {
                return typeof(TMP_FontAsset);
            }
        }

        [MenuItem("Tools/TUK/Export Font")]
        private static void Open()
        {
            OpenWindow<TUKFontExporterWindow>("TUK Font Exporter");
        }
    }
}
#endif
