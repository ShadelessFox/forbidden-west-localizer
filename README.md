# Localization mod for Horizon Forbidden West

### Installation

1. Download the latest release from the [releases page](https://github.com/ShadelessFox/forbidden-west-localizer/releases/latest)
2. Copy `winhttp.dll` and `localization.json` files to the game's root directory

### Example

The repository contains a dump of all localizable strings in the game as of version 1.5.80.0. You don't need to distribute the entire file; instead, include just the changed strings.

Example `localization.json` file:
```json
{
  "802b4818-320a-0e48-94a7-84546b9d5b73": "<icon name=ExplosiveText> Begin a new adventure"
}
```
It will change the text "New Game" to "💥 Begin a new adventure" in the main menu.
