# Script PowerShell pour Workflow Docker/Nix
# Usage: .\docker-dev.ps1 [compile|test|run|clean|shell]

param(
    [string]$Command = "shell"
)

$ProjectPath = $PSScriptRoot
$DockerImage = "nixos/nix:latest"

function Start-DevContainer {
    Write-Host "🐳 Lancement du conteneur Docker avec Nix..." -ForegroundColor Cyan
    docker run -it --rm -v "${ProjectPath}:/work" -w /work $DockerImage nix develop --command bash
}

function Compile-Project {
    Write-Host "🔨 Compilation du projet..." -ForegroundColor Cyan
    docker run -it --rm -v "${ProjectPath}:/work" -w /work $DockerImage nix develop --command bash -c `
        "meson setup build && meson compile -C build"
}

function Test-Project {
    Write-Host "✅ Exécution des tests..." -ForegroundColor Cyan
    docker run -it --rm -v "${ProjectPath}:/work" -w /work $DockerImage nix develop --command bash -c `
        "meson test -C build"
}

function Run-Examples {
    Write-Host "🎵 Exécution des exemples..." -ForegroundColor Cyan
    docker run -it --rm -v "${ProjectPath}:/work" -w /work $DockerImage nix develop --command bash -c `
        "./build/app/synthesizer_example && ./build/app/melody && ./build/app/base_project_app && ls -lh *.wav"
}

function Clean-Build {
    Write-Host "🧹 Suppression du dossier build..." -ForegroundColor Yellow
    if (Test-Path "base-project/build") {
        Remove-Item -Path "base-project/build" -Recurse -Force
        Write-Host "✓ Build supprimé" -ForegroundColor Green
    } else {
        Write-Host "ℹ️  Build n'existe pas déjà" -ForegroundColor Gray
    }
}

function Full-Build {
    Write-Host "🚀 Build complète (clean + compile + test + run)..." -ForegroundColor Cyan
    Clean-Build
    Compile-Project
    Test-Project
    Run-Examples
}

# Affichage du menu
switch ($Command.ToLower()) {
    "shell" {
        Start-DevContainer
    }
    "compile" {
        Compile-Project
    }
    "test" {
        Test-Project
    }
    "run" {
        Run-Examples
    }
    "clean" {
        Clean-Build
    }
    "full" {
        Full-Build
    }
    default {
        Write-Host "🎵 Synthétiseur Audio - Docker Workflow" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "Usage: .\docker-dev.ps1 [commande]" -ForegroundColor White
        Write-Host ""
        Write-Host "Commandes disponibles:" -ForegroundColor White
        Write-Host "  shell   - Lancer un shell interactif Docker (défaut)" -ForegroundColor Gray
        Write-Host "  compile - Compiler le projet" -ForegroundColor Gray
        Write-Host "  test    - Exécuter les tests" -ForegroundColor Gray
        Write-Host "  run     - Exécuter tous les exemples" -ForegroundColor Gray
        Write-Host "  clean   - Supprimer le dossier build/" -ForegroundColor Gray
        Write-Host "  full    - Clean + Compile + Test + Run (tout d'un coup)" -ForegroundColor Gray
        Write-Host ""
        Write-Host "Exemples:" -ForegroundColor White
        Write-Host "  .\docker-dev.ps1 shell   # Entrée interactive" -ForegroundColor DarkGray
        Write-Host "  .\docker-dev.ps1 compile # Compilation simple" -ForegroundColor DarkGray
        Write-Host "  .\docker-dev.ps1 full    # Build complet" -ForegroundColor DarkGray
    }
}
