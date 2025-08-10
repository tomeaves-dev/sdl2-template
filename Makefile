# SDL2 Template Makefile
# Provides convenient aliases for build scripts
# Works on all platforms (delegates to appropriate scripts)

.PHONY: help setup build run clean install debug release

# Default target
help:
	@echo "SDL2 Template Build Commands:"
	@echo ""
	@echo "  setup     - First-time setup (install vcpkg, dependencies, build)"
	@echo "  build     - Build the project"
	@echo "  run       - Build and run the game"
	@echo "  clean     - Clean build artifacts"
	@echo "  install   - Install/update dependencies"
	@echo ""
	@echo "  debug     - Build in debug mode"
	@echo "  release   - Build in release mode"
	@echo ""
	@echo "Alternative usage:"
	@echo "  ./scripts/setup    # Direct script execution"
	@echo "  ./scripts/build    # Direct script execution"
	@echo "  ./scripts/run      # Direct script execution"

# Main commands
setup:
	@./scripts/setup

build:
	@./scripts/build

run:
	@./scripts/run

clean:
	@./scripts/clean

install:
	@./scripts/install

# Build type variants
debug:
	@./scripts/build Debug

release:
	@./scripts/build Release

# Additional useful targets
clean-all: clean

rebuild: clean build

quick-run: run

# Development helpers
watch:
	@echo "Starting file watcher (requires fswatch)..."
	@if command -v fswatch >/dev/null 2>&1; then \
		echo "Watching for changes in src/... (Ctrl+C to stop)"; \
		fswatch -o src/ | xargs -n1 -I{} make build; \
	else \
		echo "fswatch not found. Install with:"; \
		echo "  macOS: brew install fswatch"; \
		echo "  Linux: sudo apt install fswatch"; \
	fi