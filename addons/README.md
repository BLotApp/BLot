# Blot Addons

This directory contains addons for the Blot framework. Addons are automatically cloned here when referenced in app manifests.

## Structure

- Each addon is cloned into its own subdirectory (e.g., `bxImGui/`, `bxBlend2D/`)
- Addons are managed by the BlotAddons system and referenced in app.json files
- This directory is gitignored to prevent accidentally committing cloned addons

## Development

To work on an addon locally:

1. Clone the addon repository directly into this directory
2. Make your changes
3. The BlotAddons system will detect the local copy and use it instead of cloning

## Adding New Addons

Addons are automatically discovered when referenced in app.json files:

```json
{
  "dependencies": [
    {
      "name": "bxMyAddon",
      "url": "https://github.com/username/bxMyAddon.git",
      "branch": "main"
    }
  ]
}
```

The addon will be cloned to `addons/bxMyAddon/` and integrated into the build system. 
