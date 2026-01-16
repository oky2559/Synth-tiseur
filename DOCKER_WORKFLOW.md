# Workflow Docker/Nix pour le Synthétiseur

Ce projet utilise **Docker + Nix** pour un environnement de développement portable et reproductible.

## 🚀 Démarrage Rapide

### Étape 1 : Lancer le conteneur Docker avec Nix

Depuis **PowerShell** à la racine du projet :

```powershell
cd C:\Users\louis\Desktop\ISEN\Programmation\Synth-tiseur\base-project
docker run -it --rm -v "${PWD}:/work" -w /work nixos/nix:latest nix develop --command bash
```

Vous êtes maintenant dans le conteneur avec tous les outils (Meson, Ninja, GCC, etc.)

### Étape 2 : Compiler le projet

```bash
# Configurer Meson
meson setup build

# Compiler
meson compile -C build

# Tester
meson test -C build
```

### Étape 3 : Exécuter les programmes

```bash
# Synthétiseur Example
./build/app/synthesizer_example

# Melody (Twinkle Twinkle Little Star)
./build/app/melody

# Base Project App
./build/app/base_project_app

# Afficher les fichiers générés
ls -lh *.wav
```

## 📝 Workflow Complet (One-Liner)

Pour tout faire en une seule commande :

```powershell
docker run -it --rm -v "${PWD}:/work" -w /work nixos/nix:latest nix develop --command bash -c "meson setup build && meson compile -C build && meson test -C build && ./build/app/synthesizer_example && ./build/app/melody && ./build/app/base_project_app && ls -lh *.wav"
```

## ⚠️ Règles Importantes

### ✅ À FAIRE

- Supprimer `build/` avant de changer de machine/plateforme
- Toujours développer dans Docker (pas sur Windows natif)
- Garder votre `build/` local pour la compilation

### ❌ À NE PAS FAIRE

- **Ne compilez JAMAIS** sur Windows PowerShell natif (sauf si vous savez ce que vous faites)
- **Ne committez JAMAIS** le dossier `build/` (il est dans `.gitignore`)
- Ne mélangez pas Windows et Docker pour le même projet

## 🧹 Nettoyage

Si vous avez des problèmes de compilation :

```bash
# Dans le conteneur
rm -rf build/
meson setup build
meson compile -C build
```

Ou depuis PowerShell :

```powershell
# Sur Windows
Remove-Item -Path base-project/build -Recurse -Force
# Puis relancer Docker
```

## 🐳 Prérequis

- Docker installé et en cours d'exécution
- PowerShell (Windows 7+ ou PowerShell Core)
- Environ 2-3 GB d'espace disque (pour les images Nix)

## 📂 Structure du Projet

```
base-project/
├── app/              # Programmes exécutables
├── src/              # Code source du synthétiseur
├── include/          # En-têtes
├── tests/            # Tests unitaires
├── meson.build       # Configuration Meson
└── build/            # Dossier de compilation (ignoré par git)
```

## 🎵 Fichiers Générés

Après compilation et exécution, vous trouverez :

- `example_output.wav` - Gamme musicale + accords (synthétiseur example)
- `melody_output.wav` - "Twinkle Twinkle Little Star"
- `synthesizer_output.wav` - 8 notes individuelles

## 💡 Conseils

1. **Gardez le conteneur ouvert** pendant une session de développement :
   ```bash
   docker run -it --rm -v "${PWD}:/work" -w /work nixos/nix:latest nix develop --command bash
   # Vous pouvez maintenant compiler, modifier, recompiler, etc.
   ```

2. **Les fichiers sont partagés** entre Windows et Docker via le volume `-v "${PWD}:/work"`
   - Modifiez les fichiers dans VS Code (Windows)
   - Compilez dans le conteneur (Docker)
   - Les fichiers `.wav` sont automatiquement accessibles depuis Windows

3. **Toujours faire `git status`** avant de committer pour s'assurer que `build/` n'est pas inclus

## ❓ Dépannage

**Erreur : "sh: meson: command not found"**
- → Vous êtes probablement dans PowerShell, pas dans le conteneur
- → Relancez : `docker run -it --rm -v "${PWD}:/work" -w /work nixos/nix:latest nix develop --command bash`

**Erreur : "C:\Users\...\meson: command not found"**
- → Vous aviez compilé sur Windows natif
- → Supprimez le dossier `build/` et recommencez

**Les fichiers .wav ne sont pas générés**
- → Vérifiez que vous êtes dans `/work` (pas `/work/base-project`)
- → Les fichiers sont générés dans le répertoire courant
- → Affichage : `ls -lh *.wav`

---

**Bon développement! 🎵**
