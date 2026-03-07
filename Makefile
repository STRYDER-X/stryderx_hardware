PROJECT_NAME := stryderx_hardware
LINT_EXCLUDES := third_party
CONFIG_FILE := .uncrustify.cfg

.PHONY: lint

# TODO: Should update this to trigger any linting in third_party subdirectories if applicable.
lint: ## Internal linting using the local config file
	@echo "--- [$(PROJECT_NAME)] Formatting with Config ---"
	@echo "Fixing C++ formatting..."
	@ament_uncrustify --reformat -c $(CONFIG_FILE) . --exclude $(LINT_EXCLUDES) || true
	@echo "Fixing CMake formatting..."
	# NOTE: Formatting CMake related files.
	@find . -maxdepth 3 \( -name 'CMakeLists.txt' -o -name '*.cmake' \) $(foreach dir,$(LINT_EXCLUDES),-not -path "./$(dir)/*") -exec sed -i 's/[[:space:]]*$$//' {} +

docs: ## Interface to the ROS 2 build system for documentation
	@echo "--- [$(PROJECT_NAME)] Generating API Documentation ---"
	@colcon build --packages-select $(PROJECT_NAME) --cmake-target docs