param()

$ErrorActionPreference = "Stop"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "..\..")

function ClearSerializationFiles($folder) {
    $targetDir = Join-Path $scriptDir $folder
    if (Test-Path $targetDir) {
        Get-ChildItem $targetDir -Filter "Serialization_*.d.ts" | Remove-Item -Force
    }
}

function CopySerializationFiles($metadataFolder, $schemaFolder) {
    $targetDir = Join-Path $scriptDir $schemaFolder
    if (!(Test-Path $targetDir)) {
        New-Item $targetDir -ItemType Directory | Out-Null
    }

    ClearSerializationFiles $schemaFolder
    Copy-Item (Join-Path $repoRoot "$metadataFolder\Serialization_*.d.ts") $targetDir
}

CopySerializationFiles "Test\Generated\RpcMetadata32" "DataSchema32"
CopySerializationFiles "Test\Generated\RpcMetadata64" "DataSchema64"

Write-Host "Prepared TypeScript test files successfully."
