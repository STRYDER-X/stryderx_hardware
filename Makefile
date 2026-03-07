LINT_EXCLUDES := third_party
CONFIG_FILE := .uncrustify.cfg

.PHONY: lint

# TODO: Should update this to trigger any linting in third_party subdirectories if applicable.
lint: ## Internal linting using the local config file
	@echo "--- [stryderx_hardware] Formatting with Config ---"
	@echo "Fixing C++ formatting..."
	@ament_uncrustify --reformat -c $(CONFIG_FILE) . --exclude $(LINT_EXCLUDES) || true
	@echo "Fixing CMake formatting..."
	@find . -maxdepth 3 -name 'CMakeLists.txt' $(foreach dir,$(LINT_EXCLUDES),-not -path "./$(dir)/*") -exec sed -i 's/[[:space:]]*$$//' {} +