.DEFAULT_GOAL := help
PROJECT_NAME := stryderx_hardware
LINT_EXCLUDES := third_party
CONFIG_FILE := .uncrustify.cfg

.PHONY: lint

help: ## Show this help message
	@grep -E '^(help):.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}'
	@echo ""
	@grep -E '^(lint|docs):.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}'


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