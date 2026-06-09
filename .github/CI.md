# CI/CD Documentation

## GitHub Actions Workflow

The project uses GitHub Actions to automatically build the firmware on every push and pull request.

### Workflow File

`.github/workflows/build.yml`

### Triggers

The workflow runs on:
- **Push** to `main`, `master`, or `develop` branches
- **Pull requests** targeting these branches
- **Manual trigger** via the Actions tab (workflow_dispatch)

### Build Steps

1. **Checkout** - Clone the repository
2. **Python Setup** - Install Python 3.12 with pip caching
3. **System Dependencies** - Install required Ubuntu packages (cmake, ninja, gcc, etc.)
4. **Zephyr SDK Cache** - Restore cached SDK or download if missing (~500 MB)
5. **Zephyr Workspace Cache** - Restore cached Zephyr sources or clone if missing
6. **Build** - Compile firmware using `west build`
7. **Upload Artifacts** - Store UF2/ELF/HEX files for download
8. **Build Summary** - Generate memory usage report

### Build Artifacts

After each successful build, the following files are available for download:

- `zephyr.uf2` - Firmware for UF2 bootloader (drag-and-drop flashing)
- `zephyr.elf` - ELF file with debug symbols
- `zephyr.hex` - Intel HEX format
- `zephyr.bin` - Raw binary

Artifacts are retained for **30 days**.

### Caching

To speed up builds, the workflow caches:

- **Zephyr SDK** (~500 MB) - Cached indefinitely
- **Zephyr workspace** - Cached based on `west.yml` hash

Cache keys automatically invalidate when dependencies change.

### Build Time

- **First build** (cold cache): ~15-20 minutes
- **Cached builds**: ~2-3 minutes

### Viewing Results

1. Go to the **Actions** tab in your GitHub repository
2. Click on the latest workflow run
3. View build logs and download artifacts
4. Check the **Summary** tab for memory usage statistics

### Status Badge

Add to your README:

```markdown
[![Build Firmware](https://github.com/YOUR_USERNAME/liftsense-firmware/actions/workflows/build.yml/badge.svg)](https://github.com/YOUR_USERNAME/liftsense-firmware/actions/workflows/build.yml)
```

Replace `YOUR_USERNAME` with your GitHub username.

### Manual Trigger

You can manually trigger a build:

1. Go to **Actions** tab
2. Select **Build Firmware** workflow
3. Click **Run workflow**
4. Select branch and click **Run workflow**

### Troubleshooting

**Build fails with devicetree errors:**
- Ensure `app.overlay` is in the project root
- Verify `dts/bindings/xiao-ble-battery.yaml` has correct YAML indentation

**Build fails with missing dependencies:**
- Check that all source files are committed
- Verify `CMakeLists.txt` includes all necessary sources

**Cache issues:**
- Clear cache from Settings → Actions → Caches
- Or update cache keys in the workflow file

### Local Testing

Before pushing, test locally:

```bash
./build.sh clean
```

This ensures your changes will build successfully in CI.
