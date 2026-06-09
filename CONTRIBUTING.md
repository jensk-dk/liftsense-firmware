# Contributing to LiftSense Firmware

Thank you for your interest in contributing to LiftSense!

## Development Workflow

1. **Fork** the repository
2. **Clone** your fork locally
3. **Create a branch** for your changes (`git checkout -b feature/my-feature`)
4. **Make your changes** and test locally
5. **Commit** with clear messages (`git commit -m "Add feature: ..."`)
6. **Push** to your fork (`git push origin feature/my-feature`)
7. **Open a Pull Request** against the main repository

## Before Submitting

### Local Testing

Build and test your changes locally:

```bash
./build.sh clean
```

Ensure the firmware builds without errors.

### Code Style

- Follow existing code style and formatting
- Keep functions focused and well-documented
- Add comments for complex logic
- Use descriptive variable names

### Continuous Integration

All pull requests are automatically built and tested by GitHub Actions. Your PR must pass CI checks before it can be merged:

- ✅ Build must succeed without errors
- ✅ No new compiler warnings (errors with `-Werror`)
- ✅ Memory usage should not significantly increase

You can view the CI status in the "Checks" tab of your pull request.

## Commit Messages

Write clear, descriptive commit messages:

```
Good: "Add low-pass filter to gyroscope data"
Bad:  "fix stuff"
```

## Questions?

Open an issue if you have questions or need help!

## License

By contributing, you agree that your contributions will be licensed under the Apache License 2.0.
