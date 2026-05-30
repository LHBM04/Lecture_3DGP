New-Item -ItemType Directory -Force -Path 'Resources/Materials' | Out-Null

function Write-Tag($s, $t) {
    $b = [System.Text.Encoding]::ASCII.GetBytes($t)
    $s.WriteByte($b.Length)
    $s.Write($b, 0, $b.Length)
}

function Write-Float($s, $f) {
    $b = [System.BitConverter]::GetBytes([float]$f)
    $s.Write($b, 0, 4)
}

function Create-Mat($p, $r, $g, $b, $a, $shaderPath) {
    $fs = [System.IO.File]::Create($p)
    Write-Tag $fs '<Material>:'
    Write-Tag $fs '<BaseColor>:'
    Write-Float $fs $r
    Write-Float $fs $g
    Write-Float $fs $b
    Write-Float $fs $a
    Write-Tag $fs '<Shader>:'
    Write-Tag $fs $shaderPath
    $fs.Close()
}

$shader = "Resources/Shaders/GameObject.hlsl"

Create-Mat 'Resources/Materials/Player.bin' 0.2 0.5 1.0 1.0 $shader
Create-Mat 'Resources/Materials/Wall.bin' 0.7 0.7 0.7 1.0 $shader
Create-Mat 'Resources/Materials/Stairs.bin' 0.8 0.6 0.3 1.0 $shader
Create-Mat 'Resources/Materials/Floor.bin' 0.3 0.3 0.3 1.0 $shader
