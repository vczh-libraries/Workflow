param()

$ErrorActionPreference = "Stop"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "..\..")

function CopySerializationFiles($metadataFolder, $jsonValuesFolder) {
    $targetDir = Join-Path $scriptDir $jsonValuesFolder
    if (!(Test-Path $targetDir)) {
        New-Item $targetDir -ItemType Directory | Out-Null
    }

    Get-ChildItem $targetDir -Filter "Serialization_*.d.ts" | Remove-Item -Force
    Copy-Item (Join-Path $repoRoot "$metadataFolder\Serialization_*.d.ts") $targetDir
}

CopySerializationFiles "Test\Generated\RpcMetadata32" "JsonValues32"
CopySerializationFiles "Test\Generated\RpcMetadata64" "JsonValues64"

Write-Host "Prepared TypeScript test files successfully."
