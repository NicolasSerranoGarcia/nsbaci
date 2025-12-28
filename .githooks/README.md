# Git Hooks

This directory contains custom Git hooks for the nsbaci project.

## Setup

To enable these hooks, run:

```bash
git config core.hooksPath .githooks
```

## Available Hooks

### pre-push

Automatically checks if the VERSION file matches the tag being pushed. If pushing a version tag (e.g., `v1.2.3`) and the VERSION file doesn't match, it will prompt you to update it.

**Note:** On Windows with Git Bash or MSYS2, this hook works automatically. For other environments, ensure you have a bash-compatible shell.
